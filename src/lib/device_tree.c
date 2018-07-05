/*
 * Copyright 2013 Google Inc.
 * Copyright 2018-present Facebook, Inc.
 *
 * Taken from depthcharge: src/base/device_tree.c
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

#include <assert.h>
#include <console/console.h>
#include <device_tree.h>
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

/*
 * Functions for picking apart flattened trees.
 */

int fdt_next_property(void *blob, uint32_t offset, struct fdt_property *prop)
{
	struct fdt_header *header = (struct fdt_header *)blob;
	uint32_t *ptr = (uint32_t *)(((uint8_t *)blob) + offset);

	int index = 0;
	if (be32toh(ptr[index++]) != FDT_TOKEN_PROPERTY)
		return 0;

	uint32_t size = be32toh(ptr[index++]);
	uint32_t name_offset = be32toh(ptr[index++]);
	name_offset += be32toh(header->strings_offset);

	if (prop) {
		prop->name = (char *)((uint8_t *)blob + name_offset);
		prop->data = &ptr[index];
		prop->size = size;
	}

	index += DIV_ROUND_UP(size, sizeof(uint32_t));

	return index * sizeof(uint32_t);
}

int fdt_node_name(void *blob, uint32_t offset, const char **name)
{
	uint8_t *ptr = ((uint8_t *)blob) + offset;
	if (be32toh(*(uint32_t *)ptr) != FDT_TOKEN_BEGIN_NODE)
		return 0;

	ptr += 4;
	if (name)
		*name = (char *)ptr;
	return ALIGN_UP(strlen((char *)ptr) + 1, sizeof(uint32_t)) + 4;
}



/*
 * Functions for printing flattened trees.
 */

static void print_indent(int depth)
{
	while (depth--)
		printk(BIOS_DEBUG, "  ");
}

static void print_property(struct fdt_property *prop, int depth)
{
	print_indent(depth);
	printk(BIOS_DEBUG, "prop \"%s\" (%d bytes).\n", prop->name, prop->size);
	print_indent(depth + 1);
	for (int i = 0; i < MIN(25, prop->size); i++) {
		printk(BIOS_DEBUG, "%02x ", ((uint8_t *)prop->data)[i]);
	}
	if (prop->size > 25)
		printk(BIOS_DEBUG, "...");
	printk(BIOS_DEBUG, "\n");
}

static int print_flat_node(void *blob, uint32_t start_offset, int depth)
{
	int offset = start_offset;
	const char *name;
	int size;

	size = fdt_node_name(blob, offset, &name);
	if (!size)
		return 0;
	offset += size;

	print_indent(depth);
	printk(BIOS_DEBUG, "name = %s\n", name);

	struct fdt_property prop;
	while ((size = fdt_next_property(blob, offset, &prop))) {
		print_property(&prop, depth + 1);

		offset += size;
	}

	while ((size = print_flat_node(blob, offset, depth + 1)))
		offset += size;

	return offset - start_offset + sizeof(uint32_t);
}

void fdt_print_node(void *blob, uint32_t offset)
{
	print_flat_node(blob, offset, 0);
}



/*
 * A utility function to skip past nodes in flattened trees.
 */

int fdt_skip_node(void *blob, uint32_t start_offset)
{
	int offset = start_offset;
	int size;

	const char *name;
	size = fdt_node_name(blob, offset, &name);
	if (!size)
		return 0;
	offset += size;

	while ((size = fdt_next_property(blob, offset, NULL)))
		offset += size;

	while ((size = fdt_skip_node(blob, offset)))
		offset += size;

	return offset - start_offset + sizeof(uint32_t);
}



/*
 * Functions to turn a flattened tree into an unflattened one.
 */
static struct device_tree_node *alloc_node(void)
{
	struct device_tree_node *buf = malloc(sizeof(struct device_tree_node));
	if (!buf)
		return NULL;
	memset(buf, 0, sizeof(*buf));
	return buf;
}

static struct device_tree_property *alloc_prop(void)
{
	struct device_tree_property *buf =
		malloc(sizeof(struct device_tree_property));
	if (!buf)
		return NULL;
	memset(buf, 0, sizeof(*buf));
	return buf;
}

static int fdt_unflatten_node(void *blob, uint32_t start_offset,
			      struct device_tree_node **new_node)
{
	struct list_node *last;
	int offset = start_offset;
	const char *name;
	int size;

	size = fdt_node_name(blob, offset, &name);
	if (!size)
		return 0;
	offset += size;

	struct device_tree_node *node = alloc_node();
	*new_node = node;
	if (!node)
		return 0;
	node->name = name;

	struct fdt_property fprop;
	last = &node->properties;
	while ((size = fdt_next_property(blob, offset, &fprop))) {
		struct device_tree_property *prop = alloc_prop();
		if (!prop)
			return 0;
		prop->prop = fprop;

		list_insert_after(&prop->list_node, last);
		last = &prop->list_node;

		offset += size;
	}

	struct device_tree_node *child;
	last = &node->children;
	while ((size = fdt_unflatten_node(blob, offset, &child))) {
		list_insert_after(&child->list_node, last);
		last = &child->list_node;

		offset += size;
	}

	return offset - start_offset + sizeof(uint32_t);
}

static int fdt_unflatten_map_entry(void *blob, uint32_t offset,
				   struct device_tree_reserve_map_entry **new)
{
	uint64_t *ptr = (uint64_t *)(((uint8_t *)blob) + offset);
	uint64_t start = be64toh(ptr[0]);
	uint64_t size = be64toh(ptr[1]);

	if (!size)
		return 0;

	struct device_tree_reserve_map_entry *entry = malloc(sizeof(*entry));
	if (!entry)
		return 0;
	memset(entry, 0, sizeof(*entry));
	*new = entry;
	entry->start = start;
	entry->size = size;

	return sizeof(uint64_t) * 2;
}

struct device_tree *fdt_unflatten(void *blob)
{
	struct device_tree *tree = malloc(sizeof(*tree));
	struct fdt_header *header = (struct fdt_header *)blob;
	if (!tree)
		return NULL;
	memset(tree, 0, sizeof(*tree));
	tree->header = header;

	uint32_t struct_offset = be32toh(header->structure_offset);
	uint32_t strings_offset = be32toh(header->strings_offset);
	uint32_t reserve_offset = be32toh(header->reserve_map_offset);
	uint32_t min_offset = 0;
	min_offset = MIN(struct_offset, strings_offset);
	min_offset = MIN(min_offset, reserve_offset);
	// Assume everything up to the first non-header component is part of
	// the header and needs to be preserved. This will protect us against
	// new elements being added in the future.
	tree->header_size = min_offset;

	struct device_tree_reserve_map_entry *entry;
	uint32_t offset = reserve_offset;
	int size;
	struct list_node *last = &tree->reserve_map;
	while ((size = fdt_unflatten_map_entry(blob, offset, &entry))) {
		list_insert_after(&entry->list_node, last);
		last = &entry->list_node;

		offset += size;
	}

	fdt_unflatten_node(blob, struct_offset, &tree->root);

	return tree;
}



/*
 * Functions to find the size of the device tree if it was flattened.
 */

static void dt_flat_prop_size(struct device_tree_property *prop,
			      uint32_t *struct_size, uint32_t *strings_size)
{
	// Starting token.
	*struct_size += sizeof(uint32_t);
	// Size.
	*struct_size += sizeof(uint32_t);
	// Name offset.
	*struct_size += sizeof(uint32_t);
	// Property value.
	*struct_size += ALIGN_UP(prop->prop.size, sizeof(uint32_t));

	// Property name.
	*strings_size += strlen(prop->prop.name) + 1;
}

static void dt_flat_node_size(struct device_tree_node *node,
			      uint32_t *struct_size, uint32_t *strings_size)
{
	// Starting token.
	*struct_size += sizeof(uint32_t);
	// Node name.
	*struct_size += ALIGN_UP(strlen(node->name) + 1, sizeof(uint32_t));

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node)
		dt_flat_prop_size(prop, struct_size, strings_size);

	struct device_tree_node *child;
	list_for_each(child, node->children, list_node)
		dt_flat_node_size(child, struct_size, strings_size);

	// End token.
	*struct_size += sizeof(uint32_t);
}

uint32_t dt_flat_size(struct device_tree *tree)
{
	uint32_t size = tree->header_size;
	struct device_tree_reserve_map_entry *entry;
	list_for_each(entry, tree->reserve_map, list_node)
		size += sizeof(uint64_t) * 2;
	size += sizeof(uint64_t) * 2;

	uint32_t struct_size = 0;
	uint32_t strings_size = 0;
	dt_flat_node_size(tree->root, &struct_size, &strings_size);

	size += struct_size;
	// End token.
	size += sizeof(uint32_t);

	size += strings_size;

	return size;
}



/*
 * Functions to flatten a device tree.
 */

static void dt_flatten_map_entry(struct device_tree_reserve_map_entry *entry,
				 void **map_start)
{
	((uint64_t *)*map_start)[0] = htobe64(entry->start);
	((uint64_t *)*map_start)[1] = htobe64(entry->size);
	*map_start = ((uint8_t *)*map_start) + sizeof(uint64_t) * 2;
}

static void dt_flatten_prop(struct device_tree_property *prop,
			    void **struct_start, void *strings_base,
			    void **strings_start)
{
	uint8_t *dstruct = (uint8_t *)*struct_start;
	uint8_t *dstrings = (uint8_t *)*strings_start;

	*((uint32_t *)dstruct) = htobe32(FDT_TOKEN_PROPERTY);
	dstruct += sizeof(uint32_t);

	*((uint32_t *)dstruct) = htobe32(prop->prop.size);
	dstruct += sizeof(uint32_t);

	uint32_t name_offset = (uintptr_t)dstrings - (uintptr_t)strings_base;
	*((uint32_t *)dstruct) = htobe32(name_offset);
	dstruct += sizeof(uint32_t);

	strcpy((char *)dstrings, prop->prop.name);
	dstrings += strlen(prop->prop.name) + 1;

	memcpy(dstruct, prop->prop.data, prop->prop.size);
	dstruct += ALIGN_UP(prop->prop.size, sizeof(uint32_t));

	*struct_start = dstruct;
	*strings_start = dstrings;
}

static void dt_flatten_node(struct device_tree_node *node, void **struct_start,
			    void *strings_base, void **strings_start)
{
	uint8_t *dstruct = (uint8_t *)*struct_start;
	uint8_t *dstrings = (uint8_t *)*strings_start;

	*((uint32_t *)dstruct) = htobe32(FDT_TOKEN_BEGIN_NODE);
	dstruct += sizeof(uint32_t);

	strcpy((char *)dstruct, node->name);
	dstruct += ALIGN_UP(strlen(node->name) + 1, sizeof(uint32_t));

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node)
		dt_flatten_prop(prop, (void **)&dstruct, strings_base,
				(void **)&dstrings);

	struct device_tree_node *child;
	list_for_each(child, node->children, list_node)
		dt_flatten_node(child, (void **)&dstruct, strings_base,
				(void **)&dstrings);

	*((uint32_t *)dstruct) = htobe32(FDT_TOKEN_END_NODE);
	dstruct += sizeof(uint32_t);

	*struct_start = dstruct;
	*strings_start = dstrings;
}

void dt_flatten(struct device_tree *tree, void *start_dest)
{
	uint8_t *dest = (uint8_t *)start_dest;

	memcpy(dest, tree->header, tree->header_size);
	struct fdt_header *header = (struct fdt_header *)dest;
	dest += tree->header_size;

	struct device_tree_reserve_map_entry *entry;
	list_for_each(entry, tree->reserve_map, list_node)
		dt_flatten_map_entry(entry, (void **)&dest);
	((uint64_t *)dest)[0] = ((uint64_t *)dest)[1] = 0;
	dest += sizeof(uint64_t) * 2;

	uint32_t struct_size = 0;
	uint32_t strings_size = 0;
	dt_flat_node_size(tree->root, &struct_size, &strings_size);

	uint8_t *struct_start = dest;
	header->structure_offset = htobe32(dest - (uint8_t *)start_dest);
	header->structure_size = htobe32(struct_size);
	dest += struct_size;

	*((uint32_t *)dest) = htobe32(FDT_TOKEN_END);
	dest += sizeof(uint32_t);

	uint8_t *strings_start = dest;
	header->strings_offset = htobe32(dest - (uint8_t *)start_dest);
	header->strings_size = htobe32(strings_size);
	dest += strings_size;

	dt_flatten_node(tree->root, (void **)&struct_start, strings_start,
			(void **)&strings_start);

	header->totalsize = htobe32(dest - (uint8_t *)start_dest);
}



/*
 * Functions for printing a non-flattened device tree.
 */

static void print_node(struct device_tree_node *node, int depth)
{
	print_indent(depth);
	printk(BIOS_DEBUG, "name = %s\n", node->name);

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node)
		print_property(&prop->prop, depth + 1);

	struct device_tree_node *child;
	list_for_each(child, node->children, list_node)
		print_node(child, depth + 1);
}

void dt_print_node(struct device_tree_node *node)
{
	print_node(node, 0);
}



/*
 * Functions for reading and manipulating an unflattened device tree.
 */

/*
 * Read #address-cells and #size-cells properties from a node.
 *
 * @param node		The device tree node to read from.
 * @param addrcp	Pointer to store #address-cells in, skipped if NULL.
 * @param sizecp	Pointer to store #size-cells in, skipped if NULL.
 */
void dt_read_cell_props(struct device_tree_node *node, u32 *addrcp, u32 *sizecp)
{
	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node) {
		if (addrcp && !strcmp("#address-cells", prop->prop.name))
			*addrcp = be32toh(*(u32 *)prop->prop.data);
		if (sizecp && !strcmp("#size-cells", prop->prop.name))
			*sizecp = be32toh(*(u32 *)prop->prop.data);
	}
}

/*
 * Find a node from a device tree path, relative to a parent node.
 *
 * @param parent	The node from which to start the relative path lookup.
 * @param path		An array of path component strings that will be looked
 *			up in order to find the node. Must be terminated with
 *			a NULL pointer. Example: {'firmware', 'coreboot', NULL}
 * @param addrcp	Pointer that will be updated with any #address-cells
 *			value found in the path. May be NULL to ignore.
 * @param sizecp	Pointer that will be updated with any #size-cells
 *			value found in the path. May be NULL to ignore.
 * @param create	1: Create node(s) if not found. 0: Return NULL instead.
 * @return		The found/created node, or NULL.
 */
struct device_tree_node *dt_find_node(struct device_tree_node *parent,
				      const char **path, u32 *addrcp,
				      u32 *sizecp, int create)
{
	struct device_tree_node *node, *found = NULL;

	// Update #address-cells and #size-cells for this level.
	dt_read_cell_props(parent, addrcp, sizecp);

	if (!*path)
		return parent;

	// Find the next node in the path, if it exists.
	list_for_each(node, parent->children, list_node) {
		if (!strcmp(node->name, *path)) {
			found = node;
			break;
		}
	}

	// Otherwise create it or return NULL.
	if (!found) {
		if (!create)
			return NULL;

		found = alloc_node();
		if (!found)
			return NULL;
		found->name = strdup(*path);
		if (!found->name)
			return NULL;

		list_insert_after(&found->list_node, &parent->children);
	}

	return dt_find_node(found, path + 1, addrcp, sizecp, create);
}

/*
 * Find a node from a string device tree path, relative to a parent node.
 *
 * @param parent	The node from which to start the relative path lookup.
 * @param path          A string representing a path in the device tree, with
 *			nodes separated by '/'. Example: "soc/firmware/coreboot"
 * @param addrcp	Pointer that will be updated with any #address-cells
 *			value found in the path. May be NULL to ignore.
 * @param sizecp	Pointer that will be updated with any #size-cells
 *			value found in the path. May be NULL to ignore.
 * @param create	1: Create node(s) if not found. 0: Return NULL instead.
 * @return		The found/created node, or NULL.
 *
 * It is the caller responsibility to provide the correct path string, namely
 * not starting or ending with a '/', and not having "//" anywhere in it.
 */
struct device_tree_node *dt_find_node_by_path(struct device_tree_node *parent,
					      const char *path, u32 *addrcp,
					      u32 *sizecp, int create)
{
	char *dup_path = strdup(path);
	/* Hopefully enough depth for any node. */
	const char *path_array[15];
	int i;
	char *next_slash;
	struct device_tree_node *node = NULL;

	if (!dup_path)
		return NULL;

	next_slash = dup_path;
	path_array[0] = dup_path;
	for (i = 1; i < (ARRAY_SIZE(path_array) - 1); i++) {

		next_slash = strchr(next_slash, '/');
		if (!next_slash)
			break;

		*next_slash++ = '\0';
		path_array[i] = next_slash;
	}

	if (!next_slash) {
		path_array[i] = NULL;
		node = dt_find_node(parent, path_array,
				    addrcp, sizecp, create);
	}

	free(dup_path);
	return node;
}

/*
 * Check if given node is compatible.
 *
 * @param node		The node which is to be checked for compatible property.
 * @param compat	The compatible string to match.
 * @return		1 = compatible, 0 = not compatible.
 */
static int dt_check_compat_match(struct device_tree_node *node,
				 const char *compat)
{
	struct device_tree_property *prop;

	list_for_each(prop, node->properties, list_node) {
		if (!strcmp("compatible", prop->prop.name)) {
			size_t bytes = prop->prop.size;
			const char *str = prop->prop.data;
			while (bytes > 0) {
				if (!strncmp(compat, str, bytes))
					return 1;
				size_t len = strnlen(str, bytes) + 1;
				if (bytes <= len)
					break;
				str += len;
				bytes -= len;
			}
			break;
		}
	}

	return 0;
}

/*
 * Find a node from a compatible string, in the subtree of a parent node.
 *
 * @param parent	The parent node under which to look.
 * @param compat	The compatible string to find.
 * @return		The found node, or NULL.
 */
struct device_tree_node *dt_find_compat(struct device_tree_node *parent,
					const char *compat)
{
	// Check if the parent node itself is compatible.
	if (dt_check_compat_match(parent, compat))
		return parent;

	struct device_tree_node *child;
	list_for_each(child, parent->children, list_node) {
		struct device_tree_node *found = dt_find_compat(child, compat);
		if (found)
			return found;
	}

	return NULL;
}

/*
 * Find the next compatible child of a given parent. All children upto the
 * child passed in by caller are ignored. If child is NULL, it considers all the
 * children to find the first child which is compatible.
 *
 * @param parent	The parent node under which to look.
 * @param child	The child node to start search from (exclusive). If NULL
 *                      consider all children.
 * @param compat	The compatible string to find.
 * @return		The found node, or NULL.
 */
struct device_tree_node *
dt_find_next_compat_child(struct device_tree_node *parent,
			  struct device_tree_node *child,
			  const char *compat)
{
	struct device_tree_node *next;
	int ignore = 0;

	if (child)
		ignore = 1;

	list_for_each(next, parent->children, list_node) {
		if (ignore) {
			if (child == next)
				ignore = 0;
			continue;
		}

		if (dt_check_compat_match(next, compat))
			return next;
	}

	return NULL;
}

/*
 * Find a node with matching property value, in the subtree of a parent node.
 *
 * @param parent	The parent node under which to look.
 * @param name		The property name to look for.
 * @param data		The property value to look for.
 * @param size		The property size.
 */
struct device_tree_node *dt_find_prop_value(struct device_tree_node *parent,
					    const char *name, void *data,
					    size_t size)
{
	struct device_tree_property *prop;

	/* Check if parent itself has the required property value. */
	list_for_each(prop, parent->properties, list_node) {
		if (!strcmp(name, prop->prop.name)) {
			size_t bytes = prop->prop.size;
			void *prop_data = prop->prop.data;
			if (size != bytes)
				break;
			if (!memcmp(data, prop_data, size))
				return parent;
			break;
		}
	}

	struct device_tree_node *child;
	list_for_each(child, parent->children, list_node) {
		struct device_tree_node *found = dt_find_prop_value(child, name,
								    data, size);
		if (found)
			return found;
	}
	return NULL;
}

/**
 * Find the phandle of a node.
 *
 * @param node Pointer to node containing the phandle
 * @return Zero on error, the phandle on success
 */
uint32_t dt_get_phandle(struct device_tree_node *node)
{
	uint32_t *phandle;
	size_t len;

	dt_find_bin_prop(node, "phandle", (void **)&phandle, &len);
	if (phandle != NULL && len == sizeof(*phandle))
		return be32_to_cpu(*phandle);

	dt_find_bin_prop(node, "linux,phandle", (void **)&phandle, &len);
	if (phandle != NULL && len == sizeof(*phandle))
		return be32_to_cpu(*phandle);

	return 0;
}

/*
 * Write an arbitrary sized big-endian integer into a pointer.
 *
 * @param dest		Pointer to the DT property data buffer to write.
 * @param src		The integer to write (in CPU endianess).
 * @param length	the length of the destination integer in bytes.
 */
void dt_write_int(u8 *dest, u64 src, size_t length)
{
	while (length--) {
		dest[length] = (u8)src;
		src >>= 8;
	}
}

/*
 * Delete a property by name in a given node if it exists.
 *
 * @param node		The device tree node to operate on.
 * @param name		The name of the property to delete.
 */
void dt_delete_prop(struct device_tree_node *node, const char *name)
{
	struct device_tree_property *prop;

	list_for_each(prop, node->properties, list_node) {
		if (!strcmp(prop->prop.name, name)) {
			list_remove(&prop->list_node);
			return;
		}
	}
}

/*
 * Add an arbitrary property to a node, or update it if it already exists.
 *
 * @param node		The device tree node to add to.
 * @param name		The name of the new property.
 * @param data		The raw data blob to be stored in the property.
 * @param size		The size of data in bytes.
 */
void dt_add_bin_prop(struct device_tree_node *node, const char *name,
		     void *data, size_t size)
{
	struct device_tree_property *prop;

	list_for_each(prop, node->properties, list_node) {
		if (!strcmp(prop->prop.name, name)) {
			prop->prop.data = data;
			prop->prop.size = size;
			return;
		}
	}

	prop = alloc_prop();
	if (!prop)
		return;
	list_insert_after(&prop->list_node, &node->properties);
	prop->prop.name = name;
	prop->prop.data = data;
	prop->prop.size = size;
}

/*
 * Find given string property in a node and return its content.
 *
 * @param node		The device tree node to search.
 * @param name		The name of the property.
 * @return		The found string, or NULL.
 */
const char *dt_find_string_prop(struct device_tree_node *node, const char *name)
{
	void *content;
	size_t size;

	dt_find_bin_prop(node, name, &content, &size);

	return content;
}

/*
 * Find given property in a node.
 *
 * @param node		The device tree node to search.
 * @param name		The name of the property.
 * @param data		Pointer to return raw data blob in the property.
 * @param size		Pointer to return the size of data in bytes.
 */
void dt_find_bin_prop(struct device_tree_node *node, const char *name,
		      void **data, size_t *size)
{
	struct device_tree_property *prop;

	*data = NULL;
	*size = 0;

	list_for_each(prop, node->properties, list_node) {
		if (!strcmp(prop->prop.name, name)) {
			*data = prop->prop.data;
			*size = prop->prop.size;
			return;
		}
	}
}

/*
 * Add a string property to a node, or update it if it already exists.
 *
 * @param node		The device tree node to add to.
 * @param name		The name of the new property.
 * @param str		The zero-terminated string to be stored in the property.
 */
void dt_add_string_prop(struct device_tree_node *node, const char *name,
			char *str)
{
	dt_add_bin_prop(node, name, str, strlen(str) + 1);
}

/*
 * Add a 32-bit integer property to a node, or update it if it already exists.
 *
 * @param node		The device tree node to add to.
 * @param name		The name of the new property.
 * @param val		The integer to be stored in the property.
 */
void dt_add_u32_prop(struct device_tree_node *node, const char *name, u32 val)
{
	u32 *val_ptr = malloc(sizeof(val));
	if (!val_ptr)
		return;
	*val_ptr = htobe32(val);
	dt_add_bin_prop(node, name, val_ptr, sizeof(*val_ptr));
}

/*
 * Add a 'reg' address list property to a node, or update it if it exists.
 *
 * @param node		The device tree node to add to.
 * @param addrs		Array of address values to be stored in the property.
 * @param sizes		Array of corresponding size values to 'addrs'.
 * @param count		Number of values in 'addrs' and 'sizes' (must be equal).
 * @param addr_cells	Value of #address-cells property valid for this node.
 * @param size_cells	Value of #size-cells property valid for this node.
 */
void dt_add_reg_prop(struct device_tree_node *node, u64 *addrs, u64 *sizes,
		     int count, u32 addr_cells, u32 size_cells)
{
	int i;
	size_t length = (addr_cells + size_cells) * sizeof(u32) * count;
	u8 *data = malloc(length);
	if (!data)
		return;
	u8 *cur = data;

	for (i = 0; i < count; i++) {
		dt_write_int(cur, addrs[i], addr_cells * sizeof(u32));
		cur += addr_cells * sizeof(u32);
		dt_write_int(cur, sizes[i], size_cells * sizeof(u32));
		cur += size_cells * sizeof(u32);
	}

	dt_add_bin_prop(node, "reg", data, length);
}

/*
 * Fixups to apply to a kernel's device tree before booting it.
 */

struct list_node device_tree_fixups;

int dt_apply_fixups(struct device_tree *tree)
{
	struct device_tree_fixup *fixup;
	list_for_each(fixup, device_tree_fixups, list_node) {
		assert(fixup->fixup);
		if (fixup->fixup(fixup, tree))
			return 1;
	}
	return 0;
}

int dt_set_bin_prop_by_path(struct device_tree *tree, const char *path,
			    void *data, size_t data_size, int create)
{
	char *path_copy, *prop_name;
	struct device_tree_node *dt_node;

	path_copy = strdup(path);

	if (!path_copy) {
		printk(BIOS_ERR, "Failed to allocate a copy of path %s\n",
		       path);
		return 1;
	}

	prop_name = strrchr(path_copy, '/');
	if (!prop_name) {
		printk(BIOS_ERR, "Path %s does not include '/'\n", path);
		return 1;
	}

	*prop_name++ = '\0'; /* Separate path from the property name. */

	dt_node = dt_find_node_by_path(tree->root, path_copy, NULL,
				       NULL, create);

	if (!dt_node) {
		printk(BIOS_ERR, "Failed to %s %s in the device tree\n",
		       create ? "create" : "find", path_copy);
		return 1;
	}

	dt_add_bin_prop(dt_node, prop_name, data, data_size);

	return 0;
}

/*
 * Prepare the /reserved-memory/ node.
 *
 * Technically, this can be called more than one time, to init and/or retrieve
 * the node. But dt_add_u32_prop() may leak a bit of memory if you do.
 *
 * @tree: Device tree to add/retrieve from.
 * @return: The /reserved-memory/ node (or NULL, if error).
 */
struct device_tree_node *dt_init_reserved_memory_node(struct device_tree *tree)
{
	struct device_tree_node *reserved;
	u32 addr = 0, size = 0;

	reserved = dt_find_node_by_path(tree->root, "reserved-memory", &addr,
					&size, 1);
	if (!reserved)
		return NULL;

	// Binding doc says this should have the same #{address,size}-cells as
	// the root.
	dt_add_u32_prop(reserved, "#address-cells", addr);
	dt_add_u32_prop(reserved, "#size-cells", size);
	// Binding doc says this should be empty (i.e., 1:1 mapping from root).
	dt_add_bin_prop(reserved, "ranges", NULL, 0);

	return reserved;
}
