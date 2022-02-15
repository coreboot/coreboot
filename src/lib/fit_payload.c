/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/compression.h>
#include <console/console.h>
#include <bootmem.h>
#include <cbmem.h>
#include <device/resource.h>
#include <stdlib.h>
#include <commonlib/region.h>
#include <fit.h>
#include <program_loading.h>
#include <timestamp.h>
#include <string.h>
#include <lib.h>
#include <boardid.h>

/* Pack the device_tree and place it at given position. */
static void pack_fdt(struct region *fdt, struct device_tree *dt)
{
	printk(BIOS_INFO, "FIT: Flattening FDT to %p\n",
	       (void *)fdt->offset);

	dt_flatten(dt, (void *)fdt->offset);
	prog_segment_loaded(fdt->offset, fdt->size, 0);
}

/**
 * Extract a node to given regions.
 * Returns true on error, false on success.
 */
static bool extract(struct region *region, struct fit_image_node *node)
{
	void *dst = (void *)region->offset;
	const char *comp_name;
	size_t true_size = 0;

	if (node->size == 0) {
		printk(BIOS_ERR, "The %s size is 0\n", node->name);
		return true;
	}

	switch (node->compression) {
	case CBFS_COMPRESS_NONE:
		comp_name = "Relocating uncompressed";
		break;
	case CBFS_COMPRESS_LZMA:
		comp_name = "Decompressing LZMA";
		break;
	case CBFS_COMPRESS_LZ4:
		comp_name = "Decompressing LZ4";
		break;
	default:
		printk(BIOS_ERR, "Unsupported compression\n");
		return true;
	}

	printk(BIOS_INFO, "FIT: %s %s to %p\n", comp_name, node->name, dst);

	switch (node->compression) {
	case CBFS_COMPRESS_NONE:
		memcpy(dst, node->data, node->size);
		true_size = node->size;
		break;
	case CBFS_COMPRESS_LZMA:
		timestamp_add_now(TS_ULZMA_START);
		true_size = ulzman(node->data, node->size, dst, region->size);
		timestamp_add_now(TS_ULZMA_END);
		break;
	case CBFS_COMPRESS_LZ4:
		timestamp_add_now(TS_ULZ4F_START);
		true_size = ulz4fn(node->data, node->size, dst, region->size);
		timestamp_add_now(TS_ULZ4F_END);
		break;
	default:
		return true;
	}

	if (!true_size) {
		printk(BIOS_ERR, "%s decompression failed!\n",
		       comp_name);
		return true;
	}

	return false;
}

static struct device_tree *unpack_fdt(struct fit_image_node *image_node)
{
	void *data = image_node->data;

	if (image_node->compression != CBFS_COMPRESS_NONE) {
		/* TODO: This is an ugly heuristic for how much the size will
		   expand on decompression, fix once FIT images support storing
		   the real uncompressed size. */
		struct region r = { .offset = 0, .size = image_node->size * 5 };
		data = malloc(r.size);
		r.offset = (uintptr_t)data;
		if (!data || extract(&r, image_node))
			return NULL;
	}

	return fdt_unflatten(data);
}

/**
 * Add coreboot tables, CBMEM information and optional board specific strapping
 * IDs to the device tree loaded via FIT.
 */
static void add_cb_fdt_data(struct device_tree *tree)
{
	u32 addr_cells = 1, size_cells = 1;
	u64 reg_addrs[2], reg_sizes[2];
	void *baseptr = NULL;
	size_t size = 0;

	static const char *firmware_path[] = {"firmware", NULL};
	struct device_tree_node *firmware_node = dt_find_node(tree->root,
		firmware_path, &addr_cells, &size_cells, 1);

	/* Need to add 'ranges' to the intermediate node to make 'reg' work. */
	dt_add_bin_prop(firmware_node, "ranges", NULL, 0);

	static const char *coreboot_path[] = {"coreboot", NULL};
	struct device_tree_node *coreboot_node = dt_find_node(firmware_node,
		coreboot_path, &addr_cells, &size_cells, 1);

	dt_add_string_prop(coreboot_node, "compatible", "coreboot");

	/* Fetch CB tables from cbmem */
	void *cbtable = cbmem_find(CBMEM_ID_CBTABLE);
	if (!cbtable) {
		printk(BIOS_WARNING, "FIT: No coreboot table found!\n");
		return;
	}

	/* First 'reg' address range is the coreboot table. */
	const struct lb_header *header = cbtable;
	reg_addrs[0] = (uintptr_t)header;
	reg_sizes[0] = header->header_bytes + header->table_bytes;

	/* Second is the CBMEM area (which usually includes the coreboot
	table). */
	cbmem_get_region(&baseptr, &size);
	if (!baseptr || size == 0) {
		printk(BIOS_WARNING, "FIT: CBMEM pointer/size not found!\n");
		return;
	}

	reg_addrs[1] = (uintptr_t)baseptr;
	reg_sizes[1] = size;

	dt_add_reg_prop(coreboot_node, reg_addrs, reg_sizes, 2, addr_cells,
			size_cells);

	/* Expose board ID, SKU ID, and RAM code to payload.*/
	if (board_id() != UNDEFINED_STRAPPING_ID)
		dt_add_u32_prop(coreboot_node, "board-id", board_id());

	if (sku_id() != UNDEFINED_STRAPPING_ID)
		dt_add_u32_prop(coreboot_node, "sku-id", sku_id());

	if (ram_code() != UNDEFINED_STRAPPING_ID)
		dt_add_u32_prop(coreboot_node, "ram-code", ram_code());
}

/*
 * Parse the uImage FIT, choose a configuration and extract images.
 */
void fit_payload(struct prog *payload, void *data)
{
	struct device_tree *dt = NULL;
	struct region kernel = {0}, fdt = {0}, initrd = {0};

	printk(BIOS_INFO, "FIT: Examine payload %s\n", payload->name);

	struct fit_config_node *config = fit_load(data);

	if (!config) {
		printk(BIOS_ERR, "Could not load FIT\n");
		return;
	}

	dt = unpack_fdt(config->fdt);
	if (!dt) {
		printk(BIOS_ERR, "Failed to unflatten the FDT.\n");
		return;
	}

	struct fit_overlay_chain *chain;
	list_for_each(chain, config->overlays, list_node) {
		struct device_tree *overlay = unpack_fdt(chain->overlay);
		if (!overlay || dt_apply_overlay(dt, overlay)) {
			printk(BIOS_ERR, "Failed to apply overlay %s!\n",
			       chain->overlay->name);
		}
	}

	dt_apply_fixups(dt);

	/* Insert coreboot specific information */
	add_cb_fdt_data(dt);

	/* Update device_tree */
#if defined(CONFIG_LINUX_COMMAND_LINE)
	fit_update_chosen(dt, (char *)CONFIG_LINUX_COMMAND_LINE);
#endif
	fit_update_memory(dt);

	/* Collect infos for fit_payload_arch */
	kernel.size = config->kernel->size;
	fdt.size = dt_flat_size(dt);
	initrd.size = config->ramdisk ? config->ramdisk->size : 0;

	/* Invoke arch specific payload placement and fixups */
	if (!fit_payload_arch(payload, config, &kernel, &fdt, &initrd)) {
		printk(BIOS_ERR, "Failed to find free memory region\n");
		bootmem_dump_ranges();
		return;
	}

	/* Update ramdisk location in FDT */
	if (config->ramdisk)
		fit_add_ramdisk(dt, (void *)initrd.offset, initrd.size);

	/* Repack FDT for handoff to kernel */
	pack_fdt(&fdt, dt);

	if (config->ramdisk &&
	    extract(&initrd, config->ramdisk)) {
		printk(BIOS_ERR, "Failed to extract initrd\n");
		prog_set_entry(payload, NULL, NULL);
		return;
	}

	timestamp_add_now(TS_KERNEL_DECOMPRESSION);

	if (extract(&kernel, config->kernel)) {
		printk(BIOS_ERR, "Failed to extract kernel\n");
		prog_set_entry(payload, NULL, NULL);
		return;
	}

	timestamp_add_now(TS_KERNEL_START);
}
