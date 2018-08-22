/*
 * Copyright 2013 Google Inc.
 * Copyright 2018-present Facebook, Inc.
 *
 * Taken from depthcharge: src/boot/fit.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LIB_FIT_H__
#define __LIB_FIT_H__

#include <stddef.h>
#include <stdint.h>
#include <device_tree.h>
#include <list.h>
#include <program_loading.h>

struct fit_image_node
{
	const char *name;
	const void *data;
	uint32_t size;
	int compression;

	struct list_node list_node;
};

struct fit_config_node
{
	const char *name;
	const char *kernel;
	struct fit_image_node *kernel_node;
	const char *fdt;
	struct fit_image_node *fdt_node;
	const char *ramdisk;
	struct fit_image_node *ramdisk_node;
	struct fdt_property compat;
	int compat_rank;
	int compat_pos;
	const char *compat_string;

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
