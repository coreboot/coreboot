/* Taken from depthcharge: src/boot/fit.h */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __LIB_FIT_H__
#define __LIB_FIT_H__

#include <commonlib/device_tree.h>
#include <commonlib/list.h>
#include <program_loading.h>
#include <stddef.h>
#include <stdint.h>

struct fit_image_node {
	const char *name;
	void *data;
	uint32_t size;
	int compression;

	struct list_node list_node;
};

struct fit_config_node {
	const char *name;
	struct fit_image_node *kernel;
	struct fit_image_node *fdt;
	struct list_node overlays;
	struct fit_image_node *ramdisk;
	struct fdt_property compat;
	int compat_rank;
	int compat_pos;
	const char *compat_string;

	struct list_node list_node;
};

struct fit_overlay_chain {
	struct fit_image_node *overlay;
	struct list_node list_node;
};

/*
 * Updates the cmdline in the devicetree.
 */
void fit_update_chosen(struct device_tree *tree, const char *cmd_line);

/*
 * Add a compat string to the list of supported board ids.
 * Has to be called before fit_load().
 * The most common use-case would be to implement it on board level.
 * Strings that were added first have a higher priority on finding a match.
 */
void fit_add_compat_string(const char *str);

/*
 * Updates the memory section in the devicetree.
 */
void fit_update_memory(struct device_tree *tree);

/*
 * Do architecture specific payload placements and fixups.
 * Set entrypoint and first argument (if any).
 * @param payload The payload, to set the entry point
 * @param config The extracted FIT config
 * @param kernel out-argument where to place the kernel
 * @param fdt out-argument where to place the devicetree
 * @param initrd out-argument where to place the initrd (optional)
 * @return True if all config nodes could be placed, the corresponding
 *         regions have been updated and the entry point has been set.
 *         False on error.
 */
bool fit_payload_arch(struct prog *payload, struct fit_config_node *config,
		      struct region *kernel,
		      struct region *fdt,
		      struct region *initrd);

/*
 * Unpack a FIT image into memory, choosing the right configuration through the
 * compatible string set by fit_add_compat() and return the selected config
 * node.
 */
struct fit_config_node *fit_load(void *fit);

void fit_add_ramdisk(struct device_tree *tree, void *ramdisk_addr,
		     size_t ramdisk_size);

#endif /* __LIB_FIT_H__ */
