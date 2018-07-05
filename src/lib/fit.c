/*
 * Copyright 2013 Google Inc.
 * Copyright 2018-present Facebook, Inc.
 *
 * Taken from depthcharge: src/boot/fit.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <endian.h>
#include <stdint.h>
#include <bootmem.h>
#include <stdlib.h>
#include <string.h>
#include <cbfs.h>
#include <program_loading.h>
#include <timestamp.h>
#include <memrange.h>
#include <fit.h>
#include <boardid.h>
#include <commonlib/include/commonlib/stdlib.h>

static struct list_node image_nodes;
static struct list_node config_nodes;
static struct list_node compat_strings;

struct compat_string_entry {
	const char *compat_string;
	struct list_node list_node;
};

/* Convert string to lowercase and replace '_' with '-'. */
static char *clean_compat_string(char *str)
{
	for (size_t i = 0; i < strlen(str); i++) {
		str[i] = tolower(str[i]);
		if (str[i] == '_')
			str[i] = '-';
	}

	return str;
}

static void fit_add_default_compat_strings(void)
{
	char compat_string[80] = {};

	if ((board_id() != UNDEFINED_STRAPPING_ID) &&
	    (sku_id() != UNDEFINED_STRAPPING_ID)) {
		snprintf(compat_string, sizeof(compat_string),
			 "%s,%s-rev%u-sku%u", CONFIG_MAINBOARD_VENDOR,
			 CONFIG_MAINBOARD_PART_NUMBER, board_id(), sku_id());

		fit_add_compat_string(compat_string);
	}

	if (board_id() != UNDEFINED_STRAPPING_ID) {
		snprintf(compat_string, sizeof(compat_string), "%s,%s-rev%u",
			 CONFIG_MAINBOARD_VENDOR, CONFIG_MAINBOARD_PART_NUMBER,
			 board_id());

		fit_add_compat_string(compat_string);
	}

	snprintf(compat_string, sizeof(compat_string), "%s,%s",
		 CONFIG_MAINBOARD_VENDOR, CONFIG_MAINBOARD_PART_NUMBER);

	fit_add_compat_string(compat_string);
}

static void image_node(struct device_tree_node *node)
{
	struct fit_image_node *image = xzalloc(sizeof(*image));

	image->compression = CBFS_COMPRESS_NONE;
	image->name = node->name;

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node) {
		if (!strcmp("data", prop->prop.name)) {
			image->data = prop->prop.data;
			image->size = prop->prop.size;
		} else if (!strcmp("compression", prop->prop.name)) {
			if (!strcmp("none", prop->prop.data))
				image->compression = CBFS_COMPRESS_NONE;
			else if (!strcmp("lzma", prop->prop.data))
				image->compression = CBFS_COMPRESS_LZMA;
			else if (!strcmp("lz4", prop->prop.data))
				image->compression = CBFS_COMPRESS_LZ4;
			else
				image->compression = -1;
		}
	}

	list_insert_after(&image->list_node, &image_nodes);
}

static void config_node(struct device_tree_node *node)
{
	struct fit_config_node *config = xzalloc(sizeof(*config));
	config->name = node->name;

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node) {
		if (!strcmp("kernel", prop->prop.name))
			config->kernel = prop->prop.data;
		else if (!strcmp("fdt", prop->prop.name))
			config->fdt = prop->prop.data;
		else if (!strcmp("ramdisk", prop->prop.name))
			config->ramdisk = prop->prop.data;
	}

	list_insert_after(&config->list_node, &config_nodes);
}

static void fit_unpack(struct device_tree *tree, const char **default_config)
{
	assert(tree && tree->root);

	struct device_tree_node *top;
	list_for_each(top, tree->root->children, list_node) {
		struct device_tree_node *child;
		if (!strcmp("images", top->name)) {

			list_for_each(child, top->children, list_node)
				image_node(child);

		} else if (!strcmp("configurations", top->name)) {
			struct device_tree_property *prop;
			list_for_each(prop, top->properties, list_node) {
				if (!strcmp("default", prop->prop.name) &&
						default_config)
					*default_config = prop->prop.data;
			}

			list_for_each(child, top->children, list_node)
				config_node(child);
		}
	}
}

static struct fit_image_node *find_image(const char *name)
{
	struct fit_image_node *image;
	list_for_each(image, image_nodes, list_node) {
		if (!strcmp(image->name, name))
			return image;
	}
	return NULL;
}

static int fdt_find_compat(void *blob, uint32_t start_offset,
			   struct fdt_property *prop)
{
	int offset = start_offset;
	int size;

	size = fdt_node_name(blob, offset, NULL);
	if (!size)
		return -1;
	offset += size;

	while ((size = fdt_next_property(blob, offset, prop))) {
		if (!strcmp("compatible", prop->name))
			return 0;

		offset += size;
	}

	prop->name = NULL;
	return -1;
}

static int fit_check_compat(struct fdt_property *compat_prop,
			    const char *compat_name)
{
	int bytes = compat_prop->size;
	const char *compat_str = compat_prop->data;

	for (int pos = 0; bytes && compat_str[0]; pos++) {
		if (!strncmp(compat_str, compat_name, bytes))
			return pos;
		int len = strlen(compat_str) + 1;
		compat_str += len;
		bytes -= len;
	}
	return -1;
}

void fit_update_chosen(struct device_tree *tree, char *cmd_line)
{
	const char *path[] = { "chosen", NULL };
	struct device_tree_node *node;
	node = dt_find_node(tree->root, path, NULL, NULL, 1);

	dt_add_string_prop(node, "bootargs", cmd_line);
}

void fit_add_ramdisk(struct device_tree *tree, void *ramdisk_addr,
		     size_t ramdisk_size)
{
	const char *path[] = { "chosen", NULL };
	struct device_tree_node *node;
	node = dt_find_node(tree->root, path, NULL, NULL, 1);

	/* Warning: this assumes the ramdisk is currently located below 4GiB. */
	u32 start = (uintptr_t)ramdisk_addr;
	u32 end = start + ramdisk_size;

	dt_add_u32_prop(node, "linux,initrd-start", start);
	dt_add_u32_prop(node, "linux,initrd-end", end);
}

static void update_reserve_map(uint64_t start, uint64_t end,
			       struct device_tree *tree)
{
	struct device_tree_reserve_map_entry *entry = xzalloc(sizeof(*entry));

	entry->start = start;
	entry->size = end - start;

	list_insert_after(&entry->list_node, &tree->reserve_map);
}

struct entry_params {
	unsigned addr_cells;
	unsigned size_cells;
	void *data;
};

static uint64_t max_range(unsigned size_cells)
{
	/*
	 * Split up ranges who's sizes are too large to fit in #size-cells.
	 * The largest value we can store isn't a power of two, so we'll round
	 * down to make the math easier.
	 */
	return 0x1ULL << (size_cells * 32 - 1);
}

static void update_mem_property(u64 start, u64 end, struct entry_params *params)
{
	u8 *data = (u8 *)params->data;
	u64 full_size = end - start;
	while (full_size) {
		const u64 max_size = max_range(params->size_cells);
		const u64 size = MIN(max_size, full_size);

		dt_write_int(data, start, params->addr_cells * sizeof(u32));
		data += params->addr_cells * sizeof(uint32_t);
		start += size;

		dt_write_int(data, size, params->size_cells * sizeof(u32));
		data += params->size_cells * sizeof(uint32_t);
		full_size -= size;
	}
	params->data = data;
}

struct mem_map {
	struct memranges mem;
	struct memranges reserved;
};

static bool walk_memory_table(const struct range_entry *r, void *arg)
{
	struct mem_map *arg_map = arg;

	/*
	 * Kernel likes its available memory areas at least 1MB
	 * aligned, let's trim the regions such that unaligned padding
	 * is added to reserved memory.
	 */
	if (range_entry_tag(r) == BM_MEM_RAM) {
		uint64_t new_start = ALIGN_UP(range_entry_base(r), 1 * MiB);
		uint64_t new_end = ALIGN_DOWN(range_entry_end(r), 1 * MiB);

		if (new_start != range_entry_base(r))
			memranges_insert(&arg_map->reserved,
					 range_entry_base(r),
					 new_start - range_entry_base(r),
					 BM_MEM_RESERVED);

		if (new_start != new_end)
			memranges_insert(&arg_map->mem, new_start,
					 new_end - new_start, BM_MEM_RAM);

		if (new_end != range_entry_end(r))
			memranges_insert(&arg_map->reserved, new_end,
					 range_entry_end(r) - new_end,
					 BM_MEM_RESERVED);
	} else
		memranges_insert(&arg_map->reserved, range_entry_base(r),
				 range_entry_size(r),
				 BM_MEM_RESERVED);

	return true;
}

void fit_add_compat_string(const char *str)
{
	struct compat_string_entry *compat_node;

	compat_node = xzalloc(sizeof(*compat_node));
	compat_node->compat_string = strdup(str);

	clean_compat_string((char *)compat_node->compat_string);

	list_insert_after(&compat_node->list_node, &compat_strings);
}

void fit_update_memory(struct device_tree *tree)
{
	const struct range_entry *r;
	struct device_tree_node *node;
	u32 addr_cells = 1, size_cells = 1;
	struct mem_map map;

	printk(BIOS_INFO, "FIT: Updating devicetree memory entries\n");

	dt_read_cell_props(tree->root, &addr_cells, &size_cells);

	/*
	 * First remove all existing device_type="memory" nodes, then add ours.
	 */
	list_for_each(node, tree->root->children, list_node) {
		const char *devtype = dt_find_string_prop(node, "device_type");
		if (devtype && !strcmp(devtype, "memory"))
			list_remove(&node->list_node);
	}

	node = xzalloc(sizeof(*node));

	node->name = "memory";
	list_insert_after(&node->list_node, &tree->root->children);
	dt_add_string_prop(node, "device_type", (char *)"memory");

	memranges_init_empty(&map.mem, NULL, 0);
	memranges_init_empty(&map.reserved, NULL, 0);

	bootmem_walk_os_mem(walk_memory_table, &map);

	/* CBMEM regions are both carved out and explicitly reserved. */
	memranges_each_entry(r, &map.reserved) {
		update_reserve_map(range_entry_base(r), range_entry_end(r),
				   tree);
	}

	/*
	 * Count the amount of 'reg' entries we need (account for size limits).
	 */
	size_t count = 0;
	memranges_each_entry(r, &map.mem) {
		uint64_t size = range_entry_size(r);
		uint64_t max_size = max_range(size_cells);
		count += DIV_ROUND_UP(size, max_size);
	}

	/* Allocate the right amount of space and fill up the entries. */
	size_t length = count * (addr_cells + size_cells) * sizeof(u32);

	void *data = xzalloc(length);

	struct entry_params add_params = { addr_cells, size_cells, data };
	memranges_each_entry(r, &map.mem) {
		update_mem_property(range_entry_base(r), range_entry_end(r),
				    &add_params);
	}
	assert(add_params.data - data == length);

	/* Assemble the final property and add it to the device tree. */
	dt_add_bin_prop(node, "reg", data, length);

	memranges_teardown(&map.mem);
	memranges_teardown(&map.reserved);
}

/*
 * Finds a compat string and updates the compat position and rank.
 * @param fdt_blob Pointer to FDT
 * @param config The current config node to operate on
 */
static void fit_update_compat(void *fdt_blob, struct fit_config_node *config)
{
	struct compat_string_entry *compat_node;
	struct fdt_header *fdt_header = (struct fdt_header *)fdt_blob;
	uint32_t fdt_offset = be32_to_cpu(fdt_header->structure_offset);
	size_t i = 0;

	if (!fdt_find_compat(fdt_blob, fdt_offset, &config->compat)) {
		list_for_each(compat_node, compat_strings, list_node) {
			int pos = fit_check_compat(&config->compat,
						   compat_node->compat_string);
			if (pos >= 0) {
				config->compat_pos = pos;
				config->compat_rank = i;
				config->compat_string =
					compat_node->compat_string;
				break;
			}
			i++;
		}
	}
}

struct fit_config_node *fit_load(void *fit)
{
	struct fdt_header *header = (struct fdt_header *)fit;
	struct fit_image_node *image;
	struct fit_config_node *config;
	struct compat_string_entry *compat_node;

	printk(BIOS_DEBUG, "FIT: Loading FIT from %p\n", fit);

	if (be32toh(header->magic) != FDT_HEADER_MAGIC) {
		printk(BIOS_ERR, "FIT: Bad header magic value 0x%08x.\n",
		       be32toh(header->magic));
		return NULL;
	}
	struct device_tree *tree = fdt_unflatten(fit);

	const char *default_config_name = NULL;
	struct fit_config_node *default_config = NULL;
	struct fit_config_node *compat_config = NULL;

	fit_unpack(tree, &default_config_name);

	/* List the images we found. */
	list_for_each(image, image_nodes, list_node)
		printk(BIOS_DEBUG, "FIT: Image %s has %d bytes.\n", image->name,
		       image->size);

	fit_add_default_compat_strings();

	printk(BIOS_DEBUG, "FIT: Compat preference "
	       "(lowest to highest priority) :");

	list_for_each(compat_node, compat_strings, list_node) {
		printk(BIOS_DEBUG, " %s", compat_node->compat_string);
	}
	printk(BIOS_DEBUG, "\n");
	/* Process and list the configs. */
	list_for_each(config, config_nodes, list_node) {
		if (config->kernel)
			config->kernel_node = find_image(config->kernel);
		if (config->fdt)
			config->fdt_node = find_image(config->fdt);
		if (config->ramdisk)
			config->ramdisk_node = find_image(config->ramdisk);

		if (config->ramdisk_node &&
		    config->ramdisk_node->compression < 0) {
			printk(BIOS_WARNING, "WARN: Ramdisk is compressed with "
			       "an unsupported algorithm, discarding config %s."
			       "\n", config->name);
			list_remove(&config->list_node);
			continue;
		}

		if (!config->kernel_node ||
		    (config->fdt && !config->fdt_node)) {
			printk(BIOS_DEBUG, "FIT: Missing image, discarding "
			       "config %s.\n", config->name);
			list_remove(&config->list_node);
			continue;
		}

		if (config->fdt_node) {
			if (config->fdt_node->compression !=
			    CBFS_COMPRESS_NONE) {
				printk(BIOS_DEBUG,
				       "FIT: FDT compression not yet supported,"
				       " skipping config %s.\n", config->name);
				list_remove(&config->list_node);
				continue;
			}

			config->compat_pos = -1;
			config->compat_rank = -1;

			fit_update_compat(config->fdt_node->data, config);
		}
		printk(BIOS_DEBUG, "FIT: config %s", config->name);
		if (default_config_name &&
		    !strcmp(config->name, default_config_name)) {
			printk(BIOS_DEBUG, " (default)");
			default_config = config;
		}
		if (config->fdt)
			printk(BIOS_DEBUG, ", fdt %s", config->fdt);
		if (config->ramdisk)
			printk(BIOS_DEBUG, ", ramdisk %s", config->ramdisk);
		if (config->compat.name) {
			printk(BIOS_DEBUG, ", compat");
			int bytes = config->compat.size;
			const char *compat_str = config->compat.data;
			for (int pos = 0; bytes && compat_str[0]; pos++) {
				printk(BIOS_DEBUG, " %s", compat_str);
				if (pos == config->compat_pos)
					printk(BIOS_DEBUG, " (match)");
				int len = strlen(compat_str) + 1;
				compat_str += len;
				bytes -= len;
			}

			if (config->compat_rank >= 0 && (!compat_config ||
			    config->compat_rank > compat_config->compat_rank))
				compat_config = config;
		}
		printk(BIOS_DEBUG, "\n");
	}

	struct fit_config_node *to_boot = NULL;
	if (compat_config) {
		to_boot = compat_config;
		printk(BIOS_INFO, "FIT: Choosing best match %s for compat "
		       "%s.\n", to_boot->name, to_boot->compat_string);
	} else if (default_config) {
		to_boot = default_config;
		printk(BIOS_INFO, "FIT: No match, choosing default %s.\n",
		       to_boot->name);
	} else {
		printk(BIOS_ERR, "FIT: No compatible or default configs. "
		       "Giving up.\n");
		return NULL;
	}

	return to_boot;
}
