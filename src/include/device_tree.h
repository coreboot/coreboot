/*
 * Copyright 2013 Google Inc.
 * Copyright 2018-present Facebook, Inc.
 *
 * Taken from depthcharge: src/base/device_tree.h
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

#ifndef __DEVICE_TREE_H__
#define __DEVICE_TREE_H__

#include <stdint.h>
#include <list.h>

/*
 * Flattened device tree structures/constants.
 */

struct fdt_header {
	uint32_t magic;
	uint32_t totalsize;
	uint32_t structure_offset;
	uint32_t strings_offset;
	uint32_t reserve_map_offset;

	uint32_t version;
	uint32_t last_compatible_version;

	uint32_t boot_cpuid_phys;

	uint32_t strings_size;
	uint32_t structure_size;
};

#define FDT_HEADER_MAGIC	0xd00dfeed
#define FDT_TOKEN_BEGIN_NODE	1
#define FDT_TOKEN_END_NODE	2
#define FDT_TOKEN_PROPERTY	3
#define FDT_TOKEN_END		9

struct fdt_property
{
	const char *name;
	void *data;
	uint32_t size;
};



/*
 * Unflattened device tree structures.
 */

struct device_tree_property
{
	struct fdt_property prop;

	struct list_node list_node;
};

struct device_tree_node
{
	const char *name;
	// List of struct device_tree_property-s.
	struct list_node properties;
	// List of struct device_tree_nodes.
	struct list_node children;

	struct list_node list_node;
};

struct device_tree_reserve_map_entry
{
	uint64_t start;
	uint64_t size;

	struct list_node list_node;
};

struct device_tree
{
	void *header;
	uint32_t header_size;

	struct list_node reserve_map;

	struct device_tree_node *root;
};



/*
 * Flattened device tree functions. These generally return the number of bytes
 * which were consumed reading the requested value.
 */

// Read the property, if any, at offset offset.
int fdt_next_property(void *blob, uint32_t offset, struct fdt_property *prop);
// Read the name of the node, if any, at offset offset.
int fdt_node_name(void *blob, uint32_t offset, const char **name);

void fdt_print_node(void *blob, uint32_t offset);
int fdt_skip_node(void *blob, uint32_t offset);

// Read a flattened device tree into a heirarchical structure which refers to
// the contents of the flattened tree in place. Modifying the flat tree
// invalidates the unflattened one.
struct device_tree *fdt_unflatten(void *blob);



/*
 * Unflattened device tree functions.
 */

// Figure out how big a device tree would be if it were flattened.
uint32_t dt_flat_size(struct device_tree *tree);
// Flatten a device tree into the buffer pointed to by dest.
void dt_flatten(struct device_tree *tree, void *dest);
void dt_print_node(struct device_tree_node *node);
// Read #address-cells and #size-cells properties from a node.
void dt_read_cell_props(struct device_tree_node *node, u32 *addrcp, u32 *sizecp);
// Look up or create a node relative to a parent node, through its path
// represented as an array of strings.
struct device_tree_node *dt_find_node(struct device_tree_node *parent, const char **path,
			     u32 *addrcp, u32 *sizecp, int create);
// Look up or create a node relative to a parent node, through its path
// represented as a string of '/' separated node names.
struct device_tree_node *dt_find_node_by_path(struct device_tree_node *parent, const char *path,
				     u32 *addrcp, u32 *sizecp, int create);
// Look up a node relative to a parent node, through its compatible string.
struct device_tree_node *dt_find_compat(struct device_tree_node *parent, const char *compatible);
// Look up the next child of a parent node, through its compatible string. It
// uses child pointer as the marker to find next.
struct device_tree_node *dt_find_next_compat_child(struct device_tree_node *parent,
					  struct device_tree_node *child,
					  const char *compat);
// Look up a node relative to a parent node, through its property value.
struct device_tree_node *dt_find_prop_value(struct device_tree_node *parent, const char *name,
				   void *data, size_t size);
// Return the phandle
uint32_t dt_get_phandle(struct device_tree_node *node);
// Write src into *dest as a 'length'-byte big-endian integer.
void dt_write_int(u8 *dest, u64 src, size_t length);
// Delete a property
void dt_delete_prop(struct device_tree_node *node, const char *name);
// Add different kinds of properties to a node, or update existing ones.
void dt_add_bin_prop(struct device_tree_node *node, const char *name, void *data,
		     size_t size);
void dt_add_string_prop(struct device_tree_node *node, const char *name, char *str);
void dt_add_u32_prop(struct device_tree_node *node, const char *name, u32 val);
void dt_add_reg_prop(struct device_tree_node *node, u64 *addrs, u64 *sizes,
		     int count, u32 addr_cells, u32 size_cells);
int dt_set_bin_prop_by_path(struct device_tree *tree, const char *path,
			    void *data, size_t size, int create);

void dt_find_bin_prop(struct device_tree_node *node, const char *name, void **data,
		      size_t *size);
const char *dt_find_string_prop(struct device_tree_node *node, const char *name);

/*
 * Fixups to apply to a kernel's device tree before booting it.
 */

struct device_tree_fixup
{
	/**
	 * The function which does the fixing.
	 * 0 on success, non-zero on error.
	 */
	int (*fixup)(struct device_tree_fixup *fixup,
		struct device_tree *tree);

	struct list_node list_node;
};

extern struct list_node device_tree_fixups;

/**
 * Function to apply fixups.
 * 0 on success, non-zero on error.
 */
int dt_apply_fixups(struct device_tree *tree);

/*
 * Init/retrieve the /reserved-memory/ node.
 */
struct device_tree_node *dt_init_reserved_memory_node(struct device_tree *tree);

#endif /* __DEVICE_TREE_H__ */
