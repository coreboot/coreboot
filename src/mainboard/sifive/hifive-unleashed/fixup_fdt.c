/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <console/console.h>
#include <soc/otp.h>
#include <soc/sdram.h>
#include <cbfs.h>
#include <device_tree.h>
#include <bootstate.h>
#include <mcall.h>

static void do_fixup_mac(struct device_tree_node *node)
{
	uint32_t serial = otp_read_serial();
	static unsigned char mac[6] = { 0x70, 0xb3, 0xd5, 0x92, 0xf0, 0x00 };
	if (serial != ~0) {
		mac[5] |= (serial >>  0) & 0xff;
		mac[4] |= (serial >>  8) & 0xff;
		mac[3] |= (serial >> 16) & 0xff;
	}
	dt_add_bin_prop(node, "local-mac-address", mac, 6);
}

static void fixup_mac(struct device_tree_node *parent)
{
	struct device_tree_property *prop;
	const char *name = "local-mac-address";

	list_for_each(prop, parent->properties, list_node) {
		if (!strcmp(name, prop->prop.name))
			do_fixup_mac(parent);
	}

	struct device_tree_node *child;
	list_for_each(child, parent->children, list_node) {
		fixup_mac(child);
	}
}

static void do_fixup_memory(struct device_tree_node *node)
{
	u64 addrs[1], sizes[1];
	addrs[0] = 0x80000000;
	sizes[0] = sdram_size_mb() * 1024 * 1024;
	dt_add_reg_prop(node, addrs, sizes, 1, 2, 2);
}

static void fixup_memory(struct device_tree_node *parent)
{
	struct device_tree_property *prop;
	const char *name = "device_type";
	const char *value = "memory";

	list_for_each(prop, parent->properties, list_node) {
		if (!strcmp(name, prop->prop.name)) {
			if (!strcmp(value, (char *)prop->prop.data))
				do_fixup_memory(parent);
		}
	}

	struct device_tree_node *child;
	list_for_each(child, parent->children, list_node) {
		fixup_memory(child);
	}
}

static void fixup_fdt(void *unused)
{
	void *fdt_rom;
	struct device_tree *tree;

	/* load flat dt from cbfs */
	fdt_rom = cbfs_map("fallback/DTB", NULL);

	if (fdt_rom == NULL) {
		printk(BIOS_ERR, "Unable to load fallback/DTB from CBFS\n");
		return;
	}

	/* Expand DT into a tree */
	tree = fdt_unflatten(fdt_rom);

	/* fixup tree */
	fixup_mac(tree->root);
	fixup_memory(tree->root);

	/* convert the tree to a flat dt */
	void *dt = malloc(dt_flat_size(tree));

	if (dt == NULL) {
		printk(BIOS_ERR, "Unable to allocate memory for flat device tree\n");
		return;
	}

	dt_flatten(tree, dt);

	/* update HLS */
	for (int i = 0; i < CONFIG_MAX_CPUS; i++)
		OTHER_HLS(i)->fdt = dt;
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_EXIT, fixup_fdt, NULL);
