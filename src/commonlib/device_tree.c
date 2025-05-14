/* Taken from depthcharge: src/base/device_tree.c */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/device_tree.h>
#include <ctype.h>
#include <endian.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef __COREBOOT__
#include <console/console.h>
#else
#define printk(level, ...) printf(__VA_ARGS__)
#endif
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

#define FDT_PATH_MAX_DEPTH 10 // should be a good enough upper bound
#define FDT_PATH_MAX_LEN 128 // should be a good enough upper bound
#define FDT_MAX_MEMORY_NODES 4 // should be a good enough upper bound
#define FDT_MAX_MEMORY_REGIONS 16 // should be a good enough upper bound

/*
 * libpayload's malloc() has a linear allocation complexity, which means that it
 * degrades massively if we make a few thousand small allocations. Preventing
 * that problem with a custom scratchpad is well-worth some increase in BSS
 * size (64 * 2000 + 40 * 10000 = ~1/2 MB).
 */

/* Try to give these a healthy margin above what the average kernel DT needs. */
#define LP_ALLOC_NODE_SCRATCH_COUNT 2000
#define LP_ALLOC_PROP_SCRATCH_COUNT 10000

static struct device_tree_node *alloc_node(void)
{
#ifndef __COREBOOT__
	static struct device_tree_node scratch[LP_ALLOC_NODE_SCRATCH_COUNT];
	static int counter = 0;

	if (counter < ARRAY_SIZE(scratch))
		return &scratch[counter++];
#endif
	return xzalloc(sizeof(struct device_tree_node));
}

static struct device_tree_property *alloc_prop(void)
{
#ifndef __COREBOOT__
	static struct device_tree_property scratch[LP_ALLOC_PROP_SCRATCH_COUNT];
	static int counter = 0;

	if (counter < ARRAY_SIZE(scratch))
		return &scratch[counter++];
#endif
	return xzalloc(sizeof(struct device_tree_property));
}


/*
 * internal functions used by both unflattened and flattened device tree variants
 */


static size_t read_reg_prop(struct fdt_property *prop, u32 addr_cells, u32 size_cells,
			    struct device_tree_region regions[], size_t regions_count)
{
	// we found the reg property, no need to parse all regions in 'reg'
	size_t count = prop->size / (4 * addr_cells + 4 * size_cells);
	if (count > regions_count) {
		printk(BIOS_ERR, "reg property has more entries (%zd) than regions array can hold (%zd)\n", count, regions_count);
		count = regions_count;
	}
	if (addr_cells > 2 || size_cells > 2) {
		printk(BIOS_ERR, "addr_cells (%d) or size_cells (%d) bigger than 2\n",
				  addr_cells, size_cells);
		return 0;
	}
	uint32_t *ptr = prop->data;
	for (int i = 0; i < count; i++) {
		if (addr_cells == 1)
			regions[i].addr = be32dec(ptr);
		else if (addr_cells == 2)
			regions[i].addr = be64dec(ptr);
		ptr += addr_cells;
		if (size_cells == 1)
			regions[i].size = be32dec(ptr);
		else if (size_cells == 2)
			regions[i].size = be64dec(ptr);
		ptr += size_cells;
	}

	return count; // return the number of regions found in the reg property
}


/*
 * Functions for picking apart flattened trees.
 */


static int fdt_skip_nops(const void *blob, uint32_t offset)
{
	uint32_t *ptr = (uint32_t *)(((uint8_t *)blob) + offset);

	int index = 0;
	while (be32toh(ptr[index]) == FDT_TOKEN_NOP)
		index++;

	return index * sizeof(uint32_t);
}

int fdt_next_property(const void *blob, uint32_t offset,
		      struct fdt_property *prop)
{
	struct fdt_header *header = (struct fdt_header *)blob;
	uint32_t *ptr = (uint32_t *)(((uint8_t *)blob) + offset);

	// skip NOP tokens
	offset += fdt_skip_nops(blob, offset);

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

/*
 * fdt_next_node_name  reads a node name
 *
 * @params blob    address of FDT
 * @params offset  offset to the node to read the name from
 * @params name    parameter to hold the name that has been read or NULL
 *
 * @returns  Either 0 on error or offset to the properties that come after the node name
 */
int fdt_next_node_name(const void *blob, uint32_t offset, const char **name)
{
	// skip NOP tokens
	offset += fdt_skip_nops(blob, offset);

	char *ptr = ((char *)blob) + offset;
	if (be32dec(ptr) != FDT_TOKEN_BEGIN_NODE)
		return 0;

	ptr += 4;
	if (name)
		*name = ptr;

	return ALIGN_UP(strlen(ptr) + 1, 4) + 4;
}

/*
 * A utility function to skip past nodes in flattened trees.
 */
int fdt_skip_node(const void *blob, uint32_t start_offset)
{
	uint32_t offset = start_offset;

	const char *name;
	int size = fdt_next_node_name(blob, offset, &name);
	if (!size)
		return 0;
	offset += size;

	while ((size = fdt_next_property(blob, offset, NULL)))
		offset += size;

	while ((size = fdt_skip_node(blob, offset)))
		offset += size;

	// skip NOP tokens
	offset += fdt_skip_nops(blob, offset);

	return offset - start_offset + sizeof(uint32_t);
}

/*
 * fdt_read_prop reads a property inside a node
 *
 * @params blob         address of FDT
 * @params node_offset  offset to the node to read the property from
 * @params prop_name    name of the property to read
 * @params fdt_prop     property is saved inside this parameter
 *
 * @returns  Either 0 if no property has been found or an offset that points to the location
 *           of the property
 */
u32 fdt_read_prop(const void *blob, u32 node_offset, const char *prop_name,
		  struct fdt_property *fdt_prop)
{
	u32 offset = node_offset;

	offset += fdt_next_node_name(blob, offset, NULL); // skip node name

	size_t size;
	while ((size = fdt_next_property(blob, offset, fdt_prop))) {
		if (strcmp(fdt_prop->name, prop_name) == 0)
			return offset;
		offset += size;
	}
	return 0; // property not found
}

/*
 * fdt_read_reg_prop reads the reg property inside a node
 *
 * @params blob           address of FDT
 * @params node_offset    offset to the node to read the reg property from
 * @params addr_cells     number of cells used for one address
 * @params size_cells     number of cells used for one size
 * @params regions        all regions that are read inside the reg property are saved inside
 *                        this array
 * @params regions_count  maximum number of entries that can be saved inside the regions array.
 *
 * Returns: Either 0 on error or returns the number of regions put into the regions array.
 */
u32 fdt_read_reg_prop(const void *blob, u32 node_offset, u32 addr_cells, u32 size_cells,
		      struct device_tree_region regions[], size_t regions_count)
{
	struct fdt_property prop;
	u32 offset = fdt_read_prop(blob, node_offset, "reg", &prop);

	if (!offset) {
		printk(BIOS_DEBUG, "no reg property found in node_offset: %x\n", node_offset);
		return 0;
	}

	return read_reg_prop(&prop, addr_cells, size_cells, regions, regions_count);
}

static u32 fdt_read_cell_props(const void *blob, u32 node_offset, u32 *addrcp, u32 *sizecp)
{
	struct fdt_property prop;
	u32 offset = node_offset;
	size_t size;
	while ((size = fdt_next_property(blob, offset, &prop))) {
		if (addrcp && !strcmp(prop.name, "#address-cells"))
			*addrcp = be32dec(prop.data);
		if (sizecp && !strcmp(prop.name, "#size-cells"))
			*sizecp = be32dec(prop.data);
		offset += size;
	}
	return offset;
}

/*
 * fdt_find_node searches for a node relative to another node
 *
 * @params blob  address of FDT
 *
 * @params parent_node_offset  offset to node from which to traverse the tree
 *
 * @params path  null terminated array of node names specifying a
 *               relative path (e.g: { "cpus", "cpu0", NULL })
 *
 * @params addrcp/sizecp  If any address-cells and size-cells properties are found that are
 *                        part of the parent node of the node we are looking, addrcp and sizecp
 *                        are set to these respectively.
 *
 * @returns: Either 0 if no node has been found or the offset to the node found
 */
static u32 fdt_find_node(const void *blob, u32 parent_node_offset, char **path,
			 u32 *addrcp, u32 *sizecp)
{
	if (*path == NULL)
		return parent_node_offset; // node found

	size_t size = fdt_next_node_name(blob, parent_node_offset, NULL); // skip node name

	/*
	 * get address-cells and size-cells properties while skipping the others.
	 * According to spec address-cells and size-cells are not inherited, but we
	 * intentionally follow the Linux implementation here and treat them as inheritable.
	 */
	u32 node_offset = fdt_read_cell_props(blob, parent_node_offset + size, addrcp, sizecp);

	const char *node_name;
	// walk all children nodes
	while ((size = fdt_next_node_name(blob, node_offset, &node_name))) {
		if (!strcmp(*path, node_name)) {
			// traverse one level deeper into the path
			return fdt_find_node(blob, node_offset, path + 1, addrcp, sizecp);
		}
		// node is not the correct one. skip current node
		node_offset += fdt_skip_node(blob, node_offset);
	}

	// we have searched everything and could not find a fitting node
	return 0;
}

/*
 * fdt_find_node_by_path finds a node behind a given node path
 *
 * @params blob  address of FDT
 * @params path  absolute path to the node that should be searched for
 *
 * @params addrcp/sizecp  Pointer that will be updated with any #address-cells and #size-cells
 *                        value found in the node of the node specified by node_offset. Either
 *                        may be NULL to ignore. If no #address-cells and #size-cells is found
 *                        default values of #address-cells=2 and #size-cells=1 are returned.
 *
 * @returns Either 0 on error or the offset to the node found behind the path
 */
u32 fdt_find_node_by_path(const void *blob, const char *path, u32 *addrcp, u32 *sizecp)
{
	// sanity check
	if (path[0] != '/') {
		printk(BIOS_ERR, "devicetree path must start with a /\n");
		return 0;
	}
	if (!blob) {
		printk(BIOS_ERR, "devicetree blob is NULL\n");
		return 0;
	}

	if (addrcp)
		*addrcp = 2;
	if (sizecp)
		*sizecp = 1;

	struct fdt_header *fdt_hdr = (struct fdt_header *)blob;

	/*
	 * split path into separate nodes
	 * e.g: "/cpus/cpu0" -> { "cpus", "cpu0" }
	 */
	char *path_array[FDT_PATH_MAX_DEPTH];
	size_t path_size = strlen(path);
	assert(path_size < FDT_PATH_MAX_LEN);
	char path_copy[FDT_PATH_MAX_LEN];
	memcpy(path_copy, path, path_size + 1);
	char *cur = path_copy;
	int i;
	for (i = 0; i < FDT_PATH_MAX_DEPTH; i++) {
		path_array[i] = strtok_r(NULL, "/", &cur);
		if (!path_array[i])
			break;
	}
	assert(i < FDT_PATH_MAX_DEPTH);

	return fdt_find_node(blob, be32toh(fdt_hdr->structure_offset), path_array, addrcp, sizecp);
}

/*
 * fdt_find_subnodes_by_prefix finds a node with a given prefix relative to a parent node
 *
 * @params blob  The FDT to search.
 *
 * @params node_offset  offset to the node of which the children should be searched
 *
 * @params prefix  A string to search for a node with a given prefix. This can for example
 *                 be 'cpu' to look for all nodes matching this prefix. Only children of
 *                 node_offset are searched. Therefore in order to search all nodes matching
 *                 the 'cpu' prefix, node_offset should probably point to the 'cpus' node.
 *                 An empty prefix ("") searches for all children nodes of node_offset.
 *
 * @params addrcp/sizecp  Pointer that will be updated with any #address-cells and #size-cells
 *                        value found in the node of the node specified by node_offset. Either
 *                        may be NULL to ignore. If no #address-cells and #size-cells is found
 *                        addrcp and sizecp are left untouched.
 *
 * @params results      Array of offsets pointing to each node matching the given prefix.
 * @params results_len  Number of entries allocated for the 'results' array
 *
 * @returns  offset to last node found behind path or 0 if no node has been found
 */
size_t fdt_find_subnodes_by_prefix(const void *blob, u32 node_offset, const char *prefix,
				   u32 *addrcp, u32 *sizecp, u32 *results, size_t results_len)
{
	// sanity checks
	if (!blob || !results || !prefix) {
		printk(BIOS_ERR, "%s: input parameter cannot be null/\n", __func__);
		return 0;
	}

	u32 offset = node_offset;

	// we don't care about the name of the current node
	u32 size = fdt_next_node_name(blob, offset, NULL);
	if (!size) {
		printk(BIOS_ERR, "%s: node_offset: %x does not point to a node\n",
		       __func__, node_offset);
		return 0;
	}
	offset += size;

	/*
	 * update addrcp and sizecp if the node contains an address-cells and size-cells
	 * property. Otherwise use addrcp and sizecp provided by caller.
	 */
	offset = fdt_read_cell_props(blob, offset, addrcp, sizecp);

	size_t count_results = 0;
	int prefix_len = strlen(prefix);
	const char *node_name;
	// walk all children nodes of offset
	while ((size = fdt_next_node_name(blob, offset, &node_name))) {

		// check if there is space left in the results array
		if (count_results >= results_len)
			break;

		if (!strncmp(prefix, node_name, prefix_len)) {
			// we found a node that matches the prefix
			results[count_results++] = offset;
		}
		// node does not match the prefix. skip current node
		offset += fdt_skip_node(blob, offset);
	}

	// return last occurrence
	return count_results;
}

static const char *fdt_read_alias_prop(const void *blob, const char *alias_name)
{
	u32 node_offset =  fdt_find_node_by_path(blob, "/aliases", NULL, NULL);
	if (!node_offset) {
		printk(BIOS_DEBUG, "no /aliases node found\n");
		return NULL;
	}
	struct fdt_property alias_prop;
	if (!fdt_read_prop(blob, node_offset, alias_name, &alias_prop)) {
		printk(BIOS_DEBUG, "property %s in /aliases node not found\n", alias_name);
		return NULL;
	}
	return (const char *)alias_prop.data;
}

/*
 * Find a node in the tree from a string device tree path.
 *
 * @params blob           Address to the FDT
 * @params alias_name     node name alias that should be searched for.
 * @params addrcp/sizecp  Pointer that will be updated with any #address-cells and #size-cells
 *                        value found in the node of the node specified by node_offset. Either
 *                        may be NULL to ignore. If no #address-cells and #size-cells is found
 *                        default values of #address-cells=2 and #size-cells=1 are returned.
 *
 * @returns  offset to last node found behind path or 0 if no node has been found
 */
u32 fdt_find_node_by_alias(const void *blob, const char *alias_name, u32 *addrcp, u32 *sizecp)
{
	const char *node_name = fdt_read_alias_prop(blob, alias_name);
	if (!node_name)  {
		printk(BIOS_DEBUG, "alias %s not found\n", alias_name);
		return 0;
	}

	u32 node_offset = fdt_find_node_by_path(blob, node_name, addrcp, sizecp);
	if (!node_offset) {
		// This should not happen (invalid devicetree)
		printk(BIOS_WARNING,
		       "Could not find node '%s', which alias was referring to '%s'\n",
		       node_name, alias_name);
		return 0;
	}
	return node_offset;
}


/*
 * Functions for printing flattened trees.
 */

static void print_indent(int depth)
{
	printk(BIOS_DEBUG, "%*s", depth * 8, "");
}

static void print_property(const struct fdt_property *prop, int depth)
{
	int is_string = prop->size > 0 &&
			((char *)prop->data)[prop->size - 1] == '\0';

	if (is_string) {
		for (int i = 0; i < prop->size - 1; i++) {
			if (!isprint(((char *)prop->data)[i])) {
				is_string = 0;
				break;
			}
		}
	}

	print_indent(depth);
	if (is_string) {
		printk(BIOS_DEBUG, "%s = \"%s\";\n",
		       prop->name, (const char *)prop->data);
	} else {
		printk(BIOS_DEBUG, "%s = < ", prop->name);
		for (int i = 0; i < MIN(128, prop->size); i += 4) {
			uint32_t val = 0;
			for (int j = 0; j < MIN(4, prop->size - i); j++)
				val |= ((uint8_t *)prop->data)[i + j] <<
					(24 - j * 8);
			printk(BIOS_DEBUG, "%#.2x ", val);
		}
		if (prop->size > 128)
			printk(BIOS_DEBUG, "...");
		printk(BIOS_DEBUG, ">;\n");
	}
}

static int print_flat_node(const void *blob, uint32_t start_offset, int depth)
{
	int offset = start_offset;
	const char *name;
	int size;

	size = fdt_next_node_name(blob, offset, &name);
	if (!size)
		return 0;
	offset += size;

	print_indent(depth);
	printk(BIOS_DEBUG, "%s {\n", name);

	struct fdt_property prop;
	while ((size = fdt_next_property(blob, offset, &prop))) {
		print_property(&prop, depth + 1);

		offset += size;
	}

	printk(BIOS_DEBUG, "\n");	/* empty line between props and nodes */

	while ((size = print_flat_node(blob, offset, depth + 1)))
		offset += size;

	print_indent(depth);
	printk(BIOS_DEBUG, "}\n");

	return offset - start_offset + sizeof(uint32_t);
}

void fdt_print_node(const void *blob, uint32_t offset)
{
	print_flat_node(blob, offset, 0);
}

/*
 * fdt_read_memory_regions finds memory ranges from a flat device-tree
 *
 * @params blob	          address of FDT
 * @params regions        all regions that are read inside the reg property of
 *                        memory nodes are saved inside this array
 * @params regions_count  maximum number of entries that can be saved inside
 *                        the regions array.
 *
 * Returns: Either 0 on error or returns the number of regions put into the regions array.
 */
size_t fdt_read_memory_regions(const void *blob,
			       struct device_tree_region regions[],
			       size_t regions_count)
{
	u32 node, root, addrcp, sizecp;
	u32 nodes[FDT_MAX_MEMORY_NODES] = {0};
	size_t region_idx = 0;
	size_t node_count = 0;

	if (!fdt_is_valid(blob))
		return 0;

	node = fdt_find_node_by_path(blob, "/memory",  &addrcp, &sizecp);
	if (node) {
		region_idx += fdt_read_reg_prop(blob, node, addrcp, sizecp,
						regions, regions_count);
		if (region_idx >= regions_count) {
			printk(BIOS_WARNING, "FDT: Too many memory regions\n");
			goto out;
		}
	}

	root = fdt_find_node_by_path(blob, "/",  &addrcp, &sizecp);
	node_count = fdt_find_subnodes_by_prefix(blob, root, "memory@",
						 &addrcp, &sizecp, nodes,
						 FDT_MAX_MEMORY_NODES);
	if (node_count >= FDT_MAX_MEMORY_NODES) {
		printk(BIOS_WARNING, "FDT: Too many memory nodes\n");
		/* Can still reading the regions for those we got */
	}

	for (size_t i = 0; i < MIN(node_count, FDT_MAX_MEMORY_NODES); i++) {
		region_idx += fdt_read_reg_prop(blob, nodes[i], addrcp, sizecp,
						&regions[region_idx],
						regions_count - region_idx);
		if (region_idx >= regions_count) {
			printk(BIOS_WARNING, "FDT: Too many memory regions\n");
			goto out;
		}
	}

out:
	for (size_t i = 0; i < MIN(region_idx, regions_count); i++) {
		printk(BIOS_DEBUG, "FDT: Memory region [%#llx - %#llx]\n",
		       regions[i].addr, regions[i].addr + regions[i].size);
	}

	return region_idx;
}

/*
 * fdt_get_memory_top finds top of memory from a flat device-tree
 *
 * @params blob	          address of FDT
 *
 * Returns: Either 0 on error or returns the maximum memory address
 */
uint64_t fdt_get_memory_top(const void *blob)
{
	struct device_tree_region regions[FDT_MAX_MEMORY_REGIONS] = {0};
	uint64_t top = 0;
	uint64_t total = 0;
	size_t count;

	if (!fdt_is_valid(blob))
		return 0;

	count = fdt_read_memory_regions(blob, regions, FDT_MAX_MEMORY_REGIONS);
	for (size_t i = 0; i < MIN(count, FDT_MAX_MEMORY_REGIONS); i++) {
		top = MAX(top, regions[i].addr + regions[i].size);
		total += regions[i].size;
	}

	printk(BIOS_DEBUG, "FDT: Found %u MiB of RAM\n",
	       (uint32_t)(total / MiB));

	return top;
}

/*
 * Functions to turn a flattened tree into an unflattened one.
 */

static int dt_prop_is_phandle(struct device_tree_property *prop)
{
	return !(strcmp("phandle", prop->prop.name) &&
		 strcmp("linux,phandle", prop->prop.name));
}

static int fdt_unflatten_node(const void *blob, uint32_t start_offset,
			      struct device_tree *tree,
			      struct device_tree_node **new_node)
{
	struct list_node *last;
	int offset = start_offset;
	const char *name;
	int size;

	size = fdt_next_node_name(blob, offset, &name);
	if (!size)
		return 0;
	offset += size;

	struct device_tree_node *node = alloc_node();
	*new_node = node;
	node->name = name;

	struct fdt_property fprop;
	last = &node->properties;
	while ((size = fdt_next_property(blob, offset, &fprop))) {
		struct device_tree_property *prop = alloc_prop();
		prop->prop = fprop;

		if (dt_prop_is_phandle(prop)) {
			node->phandle = be32dec(prop->prop.data);
			if (node->phandle > tree->max_phandle)
				tree->max_phandle = node->phandle;
		}

		list_insert_after(&prop->list_node, last);
		last = &prop->list_node;

		offset += size;
	}

	struct device_tree_node *child;
	last = &node->children;
	while ((size = fdt_unflatten_node(blob, offset, tree, &child))) {
		list_insert_after(&child->list_node, last);
		last = &child->list_node;

		offset += size;
	}

	return offset - start_offset + sizeof(uint32_t);
}

static int fdt_unflatten_map_entry(const void *blob, uint32_t offset,
				   struct device_tree_reserve_map_entry **new)
{
	const uint64_t *ptr = (const uint64_t *)(((uint8_t *)blob) + offset);
	const uint64_t start = be64toh(ptr[0]);
	const uint64_t size = be64toh(ptr[1]);

	if (!size)
		return 0;

	struct device_tree_reserve_map_entry *entry = xzalloc(sizeof(*entry));
	*new = entry;
	entry->start = start;
	entry->size = size;

	return sizeof(uint64_t) * 2;
}

bool fdt_is_valid(const void *blob)
{
	const struct fdt_header *header = (const struct fdt_header *)blob;

	uint32_t magic = be32toh(header->magic);
	uint32_t version = be32toh(header->version);
	uint32_t last_comp_version = be32toh(header->last_comp_version);

	if (magic != FDT_HEADER_MAGIC) {
		printk(BIOS_ERR, "Invalid device tree magic %#.8x!\n", magic);
		return false;
	}
	if (last_comp_version > FDT_SUPPORTED_VERSION) {
		printk(BIOS_ERR, "Unsupported device tree version %u(>=%u)\n",
		       version, last_comp_version);
		return false;
	}
	if (version > FDT_SUPPORTED_VERSION)
		printk(BIOS_NOTICE, "FDT version %u too new, should add support!\n",
		       version);
	return true;
}

struct device_tree *fdt_unflatten(const void *blob)
{
	struct device_tree *tree = xzalloc(sizeof(*tree));
	const struct fdt_header *header = (const struct fdt_header *)blob;
	tree->header = header;

	if (!fdt_is_valid(blob))
		return NULL;

	uint32_t struct_offset = be32toh(header->structure_offset);
	uint32_t strings_offset = be32toh(header->strings_offset);
	uint32_t reserve_offset = be32toh(header->reserve_map_offset);
	uint32_t min_offset = 0;
	min_offset = MIN(struct_offset, strings_offset);
	min_offset = MIN(min_offset, reserve_offset);
	/* Assume everything up to the first non-header component is part of
	   the header and needs to be preserved. This will protect us against
	   new elements being added in the future. */
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

	fdt_unflatten_node(blob, struct_offset, tree, &tree->root);

	return tree;
}



/*
 * Functions to find the size of the device tree if it was flattened.
 */

static void dt_flat_prop_size(struct device_tree_property *prop,
			      uint32_t *struct_size, uint32_t *strings_size)
{
	/* Starting token. */
	*struct_size += sizeof(uint32_t);
	/* Size. */
	*struct_size += sizeof(uint32_t);
	/* Name offset. */
	*struct_size += sizeof(uint32_t);
	/* Property value. */
	*struct_size += ALIGN_UP(prop->prop.size, sizeof(uint32_t));

	/* Property name. */
	*strings_size += strlen(prop->prop.name) + 1;
}

static void dt_flat_node_size(struct device_tree_node *node,
			      uint32_t *struct_size, uint32_t *strings_size)
{
	/* Starting token. */
	*struct_size += sizeof(uint32_t);
	/* Node name. */
	*struct_size += ALIGN_UP(strlen(node->name) + 1, sizeof(uint32_t));

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node)
		dt_flat_prop_size(prop, struct_size, strings_size);

	struct device_tree_node *child;
	list_for_each(child, node->children, list_node)
		dt_flat_node_size(child, struct_size, strings_size);

	/* End token. */
	*struct_size += sizeof(uint32_t);
}

uint32_t dt_flat_size(const struct device_tree *tree)
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
	/* End token. */
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

	be32enc(dstruct, FDT_TOKEN_PROPERTY);
	dstruct += sizeof(uint32_t);

	be32enc(dstruct, prop->prop.size);
	dstruct += sizeof(uint32_t);

	uint32_t name_offset = (uintptr_t)dstrings - (uintptr_t)strings_base;
	be32enc(dstruct, name_offset);
	dstruct += sizeof(uint32_t);

	strcpy((char *)dstrings, prop->prop.name);
	dstrings += strlen(prop->prop.name) + 1;

	memcpy(dstruct, prop->prop.data, prop->prop.size);
	dstruct += ALIGN_UP(prop->prop.size, sizeof(uint32_t));

	*struct_start = dstruct;
	*strings_start = dstrings;
}

static void dt_flatten_node(const struct device_tree_node *node,
			    void **struct_start, void *strings_base,
			    void **strings_start)
{
	uint8_t *dstruct = (uint8_t *)*struct_start;
	uint8_t *dstrings = (uint8_t *)*strings_start;

	be32enc(dstruct, FDT_TOKEN_BEGIN_NODE);
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

	be32enc(dstruct, FDT_TOKEN_END_NODE);
	dstruct += sizeof(uint32_t);

	*struct_start = dstruct;
	*strings_start = dstrings;
}

void dt_flatten(const struct device_tree *tree, void *start_dest)
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
	be32enc(&dest[struct_size], FDT_TOKEN_END);
	struct_size += sizeof(uint32_t);
	header->structure_size = htobe32(struct_size);
	dest += struct_size;

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

static void print_node(const struct device_tree_node *node, int depth)
{
	print_indent(depth);
	if (depth == 0)	/* root node has no name, print a starting slash */
		printk(BIOS_DEBUG, "/");
	printk(BIOS_DEBUG, "%s {\n", node->name);

	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node)
		print_property(&prop->prop, depth + 1);

	printk(BIOS_DEBUG, "\n");	/* empty line between props and nodes */

	struct device_tree_node *child;
	list_for_each(child, node->children, list_node)
		print_node(child, depth + 1);

	print_indent(depth);
	printk(BIOS_DEBUG, "};\n");
}

void dt_print_node(const struct device_tree_node *node)
{
	print_node(node, 0);
}



/*
 * Functions for reading and manipulating an unflattened device tree.
 */

/*
 * dt_read_reg_prop reads the reg property inside a node
 *
 * @params node           device tree node to read reg property from
 * @params addr_cells     number of cells used for one address
 * @params size_cells     number of cells used for one size
 * @params regions        all regions that are read inside the reg property are saved inside
 *                        this array
 * @params regions_count  maximum number of entries that can be saved inside the regions array.
 *
 * Returns: Either 0 on error or returns the number of regions put into the regions array.
 */
size_t dt_read_reg_prop(struct device_tree_node *node, u32 addr_cells, u32 size_cells,
			struct device_tree_region regions[], size_t regions_count)
{
	struct device_tree_property *prop;
	bool found = false;
	list_for_each(prop, node->properties, list_node) {
		if (!strcmp("reg", prop->prop.name)) {
			found = true;
			break;
		}
	}

	if (!found) {
		printk(BIOS_DEBUG, "no reg property found\n");
		return 0;
	}
	return read_reg_prop(&prop->prop, addr_cells, size_cells, regions, regions_count);
}

/*
 * Read #address-cells and #size-cells properties from a node.
 *
 * @param node		The device tree node to read from.
 * @param addrcp	Pointer to store #address-cells in, skipped if NULL.
 * @param sizecp	Pointer to store #size-cells in, skipped if NULL.
 */
void dt_read_cell_props(const struct device_tree_node *node, u32 *addrcp,
			u32 *sizecp)
{
	struct device_tree_property *prop;
	list_for_each(prop, node->properties, list_node) {
		if (addrcp && !strcmp("#address-cells", prop->prop.name))
			*addrcp = be32dec(prop->prop.data);
		if (sizecp && !strcmp("#size-cells", prop->prop.name))
			*sizecp = be32dec(prop->prop.data);
	}
}

static struct device_tree_node *_dt_find_node(struct device_tree_node *parent,
					      const char *const *path, u32 *addrcp,
					      u32 *sizecp, int create)
{
	struct device_tree_node *node, *found = NULL;

	if (!*path)
		return parent;

	/* Update #address-cells and #size-cells for the parent level (cells
	   properties always count for the direct children of their node). */
	if (addrcp || sizecp)
		dt_read_cell_props(parent, addrcp, sizecp);

	/* Find the next node in the path, if it exists. */
	list_for_each(node, parent->children, list_node) {
		if (!strcmp(node->name, *path)) {
			found = node;
			break;
		}
	}

	/* Otherwise create it or return NULL. */
	if (!found) {
		if (!create)
			return NULL;

		found = alloc_node();
		found->name = strdup(*path);
		if (!found->name)
			return NULL;

		list_insert_after(&found->list_node, &parent->children);
	}

	return _dt_find_node(found, path + 1, addrcp, sizecp, create);
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
				      const char *const *path, u32 *addrcp,
				      u32 *sizecp, int create)
{
	/* Initialize cells to default values according to FDT spec. */
	if (addrcp)
		*addrcp = 2;

	if (sizecp)
		*sizecp = 1;

	return _dt_find_node(parent, path, addrcp, sizecp, create);
}

/*
 * Find a node in the tree from a string device tree path.
 *
 * @param tree		The device tree to search.
 * @param path          A string representing a path in the device tree, with
 *			nodes separated by '/'. Example: "/firmware/coreboot"
 * @param addrcp	Pointer that will be updated with any #address-cells
 *			value found in the path. May be NULL to ignore.
 * @param sizecp	Pointer that will be updated with any #size-cells
 *			value found in the path. May be NULL to ignore.
 * @param create	1: Create node(s) if not found. 0: Return NULL instead.
 * @return		The found/created node, or NULL.
 *
 * It is the caller responsibility to provide a path string that doesn't end
 * with a '/' and doesn't contain any "//". If the path does not start with a
 * '/', the first segment is interpreted as an alias. */
struct device_tree_node *dt_find_node_by_path(struct device_tree *tree,
					      const char *path, u32 *addrcp,
					      u32 *sizecp, int create)
{
	char *sub_path;
	char *duped_str;
	struct device_tree_node *parent;
	char *next_slash;
	/* Hopefully enough depth for any node. */
	const char *path_array[15];
	int i;
	struct device_tree_node *node = NULL;

	if (path[0] == '/') { /* regular path */
		if (path[1] == '\0') {	/* special case: "/" is root node */
			if (addrcp || sizecp)
				dt_read_cell_props(tree->root, addrcp, sizecp);
			return tree->root;
		}

		sub_path = duped_str = strdup(&path[1]);
		if (!sub_path)
			return NULL;

		parent = tree->root;
	} else { /* alias */
		char *alias;

		alias = duped_str = strdup(path);
		if (!alias)
			return NULL;

		sub_path = strchr(alias, '/');
		if (sub_path)
			*sub_path = '\0';

		parent = dt_find_node_by_alias(tree, alias);
		if (!parent) {
			printk(BIOS_DEBUG,
			       "Could not find node '%s', alias '%s' does not exist\n",
			       path, alias);
			free(duped_str);
			return NULL;
		}

		if (!sub_path) {
			/* it's just the alias, no sub-path */
			free(duped_str);
			return parent;
		}

		sub_path++;
	}

	next_slash = sub_path;
	path_array[0] = sub_path;
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

	free(duped_str);
	return node;
}

/*
 * Find a node from an alias
 *
 * @param tree		The device tree.
 * @param alias		The alias name.
 * @return		The found node, or NULL.
 */
struct device_tree_node *dt_find_node_by_alias(struct device_tree *tree,
					       const char *alias)
{
	struct device_tree_node *node;
	const char *alias_path;

	node = dt_find_node_by_path(tree, "/aliases", NULL, NULL, 0);
	if (!node)
		return NULL;

	alias_path = dt_find_string_prop(node, alias);
	if (!alias_path)
		return NULL;

	return dt_find_node_by_path(tree, alias_path, NULL, NULL, 0);
}

struct device_tree_node *dt_find_node_by_phandle(struct device_tree_node *root,
						 uint32_t phandle)
{
	if (!root)
		return NULL;

	if (root->phandle == phandle)
		return root;

	struct device_tree_node *node;
	struct device_tree_node *result;
	list_for_each(node, root->children, list_node) {
		result = dt_find_node_by_phandle(node, phandle);
		if (result)
			return result;
	}

	return NULL;
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
	/* Check if the parent node itself is compatible. */
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
 * Find the next compatible child of a given parent. All children up to the
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
			const void *prop_data = prop->prop.data;
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

/*
 * Write an arbitrary sized big-endian integer into a pointer.
 *
 * @param dest		Pointer to the DT property data buffer to write.
 * @param src		The integer to write (in CPU endianness).
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
const char *dt_find_string_prop(const struct device_tree_node *node,
				const char *name)
{
	const void *content;
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
void dt_find_bin_prop(const struct device_tree_node *node, const char *name,
		      const void **data, size_t *size)
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
			const char *str)
{
	dt_add_bin_prop(node, name, (char *)str, strlen(str) + 1);
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
	u32 *val_ptr = xmalloc(sizeof(val));
	*val_ptr = htobe32(val);
	dt_add_bin_prop(node, name, val_ptr, sizeof(*val_ptr));
}

/*
 * Add a 64-bit integer property to a node, or update it if it already exists.
 *
 * @param node		The device tree node to add to.
 * @param name		The name of the new property.
 * @param val		The integer to be stored in the property.
 */
void dt_add_u64_prop(struct device_tree_node *node, const char *name, u64 val)
{
	u64 *val_ptr = xmalloc(sizeof(val));
	*val_ptr = htobe64(val);
	dt_add_bin_prop(node, name, val_ptr, sizeof(*val_ptr));
}

/*
 * Add a 'reg' address list property to a node, or update it if it exists.
 *
 * @param node		The device tree node to add to.
 * @param regions       Array of address values to be stored in the property.
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
	u8 *data = xmalloc(length);
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
		free(path_copy);
		printk(BIOS_ERR, "Path %s does not include '/'\n", path);
		return 1;
	}

	*prop_name++ = '\0'; /* Separate path from the property name. */

	dt_node = dt_find_node_by_path(tree, path_copy, NULL,
				       NULL, create);

	if (!dt_node) {
		printk(BIOS_ERR, "Failed to %s %s in the device tree\n",
		       create ? "create" : "find", path_copy);
		free(path_copy);
		return 1;
	}

	dt_add_bin_prop(dt_node, prop_name, data, data_size);
	free(path_copy);

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

	reserved = dt_find_node_by_path(tree, "/reserved-memory", &addr,
					&size, 1);
	if (!reserved)
		return NULL;

	/* Binding doc says this should have the same #{address,size}-cells as
	   the root. */
	dt_add_u32_prop(reserved, "#address-cells", addr);
	dt_add_u32_prop(reserved, "#size-cells", size);
	/* Binding doc says this should be empty (1:1 mapping from root). */
	dt_add_bin_prop(reserved, "ranges", NULL, 0);

	return reserved;
}

/*
 * Increment a single phandle in prop at a given offset by a given adjustment.
 *
 * @param prop		Property whose phandle should be adjusted.
 * @param adjustment	Value that should be added to the existing phandle.
 * @param offset	Byte offset of the phandle in the property data.
 *
 * @return		New phandle value, or 0 on error.
 */
static uint32_t dt_adjust_phandle(struct device_tree_property *prop,
				  uint32_t adjustment, uint32_t offset)
{
	if (offset + 4 > prop->prop.size)
		return 0;

	uint32_t phandle = be32dec(prop->prop.data + offset);
	if (phandle == 0 ||
	    phandle == FDT_PHANDLE_ILLEGAL ||
	    phandle == 0xffffffff)
		return 0;

	phandle += adjustment;
	if (phandle >= FDT_PHANDLE_ILLEGAL)
		return 0;

	be32enc(prop->prop.data + offset, phandle);
	return phandle;
}

/*
 * Adjust all phandles in subtree by adding a new base offset.
 *
 * @param node		Root node of the subtree to work on.
 * @param base		New phandle base to be added to all phandles.
 *
 * @return		New highest phandle in the subtree, or 0 on error.
 */
static uint32_t dt_adjust_all_phandles(struct device_tree_node *node,
				       uint32_t base)
{
	uint32_t new_max = MAX(base, 1);  /* make sure we don't return 0 */
	struct device_tree_property *prop;
	struct device_tree_node *child;

	if (!node)
		return new_max;

	list_for_each(prop, node->properties, list_node)
		if (dt_prop_is_phandle(prop)) {
			node->phandle = dt_adjust_phandle(prop, base, 0);
			if (!node->phandle)
				return 0;
			new_max = MAX(new_max, node->phandle);
		}  /* no break -- can have more than one phandle prop */

	list_for_each(child, node->children, list_node)
		new_max = MAX(new_max, dt_adjust_all_phandles(child, base));

	return new_max;
}

/*
 * Apply a /__local_fixup__ subtree to the corresponding overlay subtree.
 *
 * @param node		Root node of the overlay subtree to fix up.
 * @param node		Root node of the /__local_fixup__ subtree.
 * @param base		Adjustment that was added to phandles in the overlay.
 *
 * @return		0 on success, -1 on error.
 */
static int dt_fixup_locals(struct device_tree_node *node,
		    struct device_tree_node *fixup, uint32_t base)
{
	struct device_tree_property *prop;
	struct device_tree_property *fixup_prop;
	struct device_tree_node *child;
	struct device_tree_node *fixup_child;
	int i;

	/*
	 * For local fixups the /__local_fixup__ subtree contains the same node
	 * hierarchy as the main tree we're fixing up. Each property contains
	 * the fixup offsets for the respective property in the main tree. For
	 * each property in the fixup node, find the corresponding property in
	 * the base node and apply fixups to all offsets it specifies.
	 */
	list_for_each(fixup_prop, fixup->properties, list_node) {
		struct device_tree_property *base_prop = NULL;
		list_for_each(prop, node->properties, list_node)
			if (!strcmp(prop->prop.name, fixup_prop->prop.name)) {
				base_prop = prop;
				break;
			}

		/* We should always find a corresponding base prop for a fixup,
		   and fixup props contain a list of 32-bit fixup offsets. */
		if (!base_prop || fixup_prop->prop.size % sizeof(uint32_t))
			return -1;

		for (i = 0; i < fixup_prop->prop.size; i += sizeof(uint32_t))
			if (!dt_adjust_phandle(base_prop, base, be32dec(
					fixup_prop->prop.data + i)))
				return -1;
	}

	/* Now recursively descend both the base tree and the /__local_fixups__
	   subtree in sync to apply all fixups. */
	list_for_each(fixup_child, fixup->children, list_node) {
		struct device_tree_node *base_child = NULL;
		list_for_each(child, node->children, list_node)
			if (!strcmp(child->name, fixup_child->name)) {
				base_child = child;
				break;
			}

		/* All fixup nodes should have a corresponding base node. */
		if (!base_child)
			return -1;

		if (dt_fixup_locals(base_child, fixup_child, base) < 0)
			return -1;
	}

	return 0;
}

/*
 * Update all /__symbols__ properties in an overlay that start with
 * "/fragment@X/__overlay__" with corresponding path prefix in the base tree.
 *
 * @param symbols	/__symbols__ done to update.
 * @param fragment	/fragment@X node that references to should be updated.
 * @param base_path	Path of base tree node that the fragment overlaid.
 */
static void dt_fix_symbols(struct device_tree_node *symbols,
			   struct device_tree_node *fragment,
			   const char *base_path)
{
	struct device_tree_property *prop;
	char buf[512]; /* Should be enough for maximum DT path length? */
	char node_path[64]; /* easily enough for /fragment@XXXX/__overlay__ */

	if (!symbols) /* If the overlay has no /__symbols__ node, we're done! */
		return;

	int len = snprintf(node_path, sizeof(node_path), "/%s/__overlay__",
			   fragment->name);

	list_for_each(prop, symbols->properties, list_node)
		if (!strncmp(prop->prop.data, node_path, len)) {
			prop->prop.size = snprintf(buf, sizeof(buf), "%s%s",
				base_path, (char *)prop->prop.data + len) + 1;
			free(prop->prop.data);
			prop->prop.data = strdup(buf);
		}
}

/*
 * Fix up overlay according to a property in /__fixup__. If the fixed property
 * is a /fragment@X:target, also update /__symbols__ references to fragment.
 *
 * @params overlay	Overlay to fix up.
 * @params fixup	/__fixup__ property.
 * @params phandle	phandle value to insert where the fixup points to.
 * @params base_path	Path to the base DT node that the fixup points to.
 * @params overlay_symbols /__symbols__ node of the overlay.
 *
 * @return		0 on success, -1 on error.
 */
static int dt_fixup_external(struct device_tree *overlay,
			     struct device_tree_property *fixup,
			     uint32_t phandle, const char *base_path,
			     struct device_tree_node *overlay_symbols)
{
	struct device_tree_property *prop;

	/* External fixup properties are encoded as "<path>:<prop>:<offset>". */
	char *entry = fixup->prop.data;
	while ((void *)entry < fixup->prop.data + fixup->prop.size) {
		/* okay to destroy fixup property value, won't need it again */
		char *node_path = entry;
		entry = strchr(node_path, ':');
		if (!entry)
			return -1;
		*entry++ = '\0';

		char *prop_name = entry;
		entry = strchr(prop_name, ':');
		if (!entry)
			return -1;
		*entry++ = '\0';

		struct device_tree_node *ovl_node = dt_find_node_by_path(
			overlay, node_path, NULL, NULL, 0);
		if (!ovl_node || !isdigit(*entry))
			return -1;

		struct device_tree_property *ovl_prop = NULL;
		list_for_each(prop, ovl_node->properties, list_node)
			if (!strcmp(prop->prop.name, prop_name)) {
				ovl_prop = prop;
				break;
			}

		/* Move entry to first char after number, must be a '\0'. */
		uint32_t offset = skip_atoi(&entry);
		if (!ovl_prop || offset + 4 > ovl_prop->prop.size || entry[0])
			return -1;
		entry++;  /* jump over '\0' to potential next fixup */

		be32enc(ovl_prop->prop.data + offset, phandle);

		/* If this is a /fragment@X:target property, update references
		   to this fragment in the overlay __symbols__ now. */
		if (offset == 0 && !strcmp(prop_name, "target") &&
		    !strchr(node_path + 1, '/')) /* only toplevel nodes */
			dt_fix_symbols(overlay_symbols, ovl_node, base_path);
	}

	return 0;
}

/*
 * Apply all /__fixup__ properties in the overlay. This will destroy the
 * property data in /__fixup__ and it should not be accessed again.
 *
 * @params tree		Base device tree that the overlay updates.
 * @params symbols	/__symbols__ node of the base device tree.
 * @params overlay	Overlay to fix up.
 * @params fixups	/__fixup__ node in the overlay.
 * @params overlay_symbols /__symbols__ node of the overlay.
 *
 * @return		0 on success, -1 on error.
 */
static int dt_fixup_all_externals(struct device_tree *tree,
				  struct device_tree_node *symbols,
				  struct device_tree *overlay,
				  struct device_tree_node *fixups,
				  struct device_tree_node *overlay_symbols)
{
	struct device_tree_property *fix;

	/* If we have any external fixups, base tree must have /__symbols__. */
	if (!symbols)
		return -1;

	/*
	 * Unlike /__local_fixups__, /__fixups__ is not a whole subtree that
	 * mirrors the node hierarchy. It's just a directory of fixup properties
	 * that each directly contain all information necessary to apply them.
	 */
	list_for_each(fix, fixups->properties, list_node) {
		/* The name of a fixup property is the label of the node we want
		   a property to phandle-reference. Look up in /__symbols__. */
		const char *path = dt_find_string_prop(symbols, fix->prop.name);
		if (!path)
			return -1;

		/* Find node the label pointed to figure out its phandle. */
		struct device_tree_node *node = dt_find_node_by_path(tree, path,
			NULL, NULL, 0);
		if (!node)
			return -1;

		/* Write into the overlay property(s) pointing to that node. */
		if (dt_fixup_external(overlay, fix, node->phandle,
				      path, overlay_symbols) < 0)
			return -1;
	}

	return 0;
}

/*
 * Copy all nodes and properties from one DT subtree into another. This is a
 * shallow copy so both trees will point to the same property data afterwards.
 *
 * @params dst		Destination subtree to copy into.
 * @params src		Source subtree to copy from.
 * @params upd		1 to overwrite same-name properties, 0 to discard them.
 */
static void dt_copy_subtree(struct device_tree_node *dst,
			    struct device_tree_node *src, int upd)
{
	struct device_tree_property *prop;
	struct device_tree_property *src_prop;
	list_for_each(src_prop, src->properties, list_node) {
		if (dt_prop_is_phandle(src_prop) ||
		    !strcmp(src_prop->prop.name, "name")) {
			printk(BIOS_DEBUG,
			       "WARNING: ignoring illegal overlay prop '%s'\n",
			       src_prop->prop.name);
			continue;
		}

		struct device_tree_property *dst_prop = NULL;
		list_for_each(prop, dst->properties, list_node)
			if (!strcmp(prop->prop.name, src_prop->prop.name)) {
				dst_prop = prop;
				break;
			}

		if (dst_prop) {
			if (!upd) {
				printk(BIOS_DEBUG,
				       "WARNING: ignoring prop update '%s'\n",
				       src_prop->prop.name);
				continue;
			}
		} else {
			dst_prop = alloc_prop();
			list_insert_after(&dst_prop->list_node,
					  &dst->properties);
		}

		dst_prop->prop = src_prop->prop;
	}

	struct device_tree_node *node;
	struct device_tree_node *src_node;
	list_for_each(src_node, src->children, list_node) {
		struct device_tree_node *dst_node = NULL;
		list_for_each(node, dst->children, list_node)
			if (!strcmp(node->name, src_node->name)) {
				dst_node = node;
				break;
			}

		if (!dst_node) {
			dst_node = alloc_node();
			*dst_node = *src_node;
			list_insert_after(&dst_node->list_node, &dst->children);
		} else {
			dt_copy_subtree(dst_node, src_node, upd);
		}
	}
}

/*
 * Apply an overlay /fragment@X node to a base device tree.
 *
 * @param tree		Base device tree.
 * @param fragment	/fragment@X node.
 * @params overlay_symbols /__symbols__ node of the overlay.
 *
 * @return		0 on success, -1 on error.
 */
static int dt_import_fragment(struct device_tree *tree,
			      struct device_tree_node *fragment,
			      struct device_tree_node *overlay_symbols)
{
	/* The actual overlaid nodes/props are in an __overlay__ child node. */
	static const char *overlay_path[] = { "__overlay__", NULL };
	struct device_tree_node *overlay = dt_find_node(fragment, overlay_path,
							NULL, NULL, 0);

	/* If it doesn't have an __overlay__ child, it's not a fragment. */
	if (!overlay)
		return 0;

	/* Target node of the fragment can be given by path or by phandle. */
	struct device_tree_property *prop;
	struct device_tree_property *phandle = NULL;
	struct device_tree_property *path = NULL;
	list_for_each(prop, fragment->properties, list_node) {
		if (!strcmp(prop->prop.name, "target")) {
			phandle = prop;
			break; /* phandle target has priority, stop looking */
		}
		if (!strcmp(prop->prop.name, "target-path"))
			path = prop;
	}

	struct device_tree_node *target = NULL;
	if (phandle) {
		if (phandle->prop.size != sizeof(uint32_t))
			return -1;
		target = dt_find_node_by_phandle(tree->root,
						 be32dec(phandle->prop.data));
		/* Symbols already updated as part of dt_fixup_external(). */
	} else if (path) {
		target = dt_find_node_by_path(tree, path->prop.data,
					      NULL, NULL, 0);
		dt_fix_symbols(overlay_symbols, fragment, path->prop.data);
	}
	if (!target)
		return -1;

	dt_copy_subtree(target, overlay, 1);
	return 0;
}

/*
 * Apply a device tree overlay to a base device tree. This will
 * destroy/incorporate the overlay data, so it should not be freed or reused.
 * See dtc.git/Documentation/dt-object-internal.txt for overlay format details.
 *
 * @param tree		Unflattened base device tree to add the overlay into.
 * @param overlay	Unflattened overlay device tree to apply to the base.
 *
 * @return		0 on success, -1 on error.
 */
int dt_apply_overlay(struct device_tree *tree, struct device_tree *overlay)
{
	/*
	 * First, we need to make sure phandles inside the overlay don't clash
	 * with those in the base tree. We just define the highest phandle value
	 * in the base tree as the "phandle offset" for this overlay and
	 * increment all phandles in it by that value.
	 */
	uint32_t phandle_base = tree->max_phandle;
	uint32_t new_max = dt_adjust_all_phandles(overlay->root, phandle_base);
	if (!new_max) {
		printk(BIOS_ERR, "invalid phandles in overlay\n");
		return -1;
	}
	tree->max_phandle = new_max;

	/* Now that we changed phandles in the overlay, we need to update any
	   nodes referring to them. Those are listed in /__local_fixups__. */
	struct device_tree_node *local_fixups = dt_find_node_by_path(overlay,
					"/__local_fixups__", NULL, NULL, 0);
	if (local_fixups && dt_fixup_locals(overlay->root, local_fixups,
					    phandle_base) < 0) {
		printk(BIOS_ERR, "invalid local fixups in overlay\n");
		return -1;
	}

	/*
	 * Besides local phandle references (from nodes within the overlay to
	 * other nodes within the overlay), the overlay may also contain phandle
	 * references to the base tree. These are stored with invalid values and
	 * must be updated now. /__symbols__ contains a list of all labels in
	 * the base tree, and /__fixups__ describes all nodes in the overlay
	 * that contain external phandle references.
	 * We also take this opportunity to update all /fragment@X/__overlay__/
	 * prefixes in the overlay's /__symbols__ node to the correct path that
	 * the fragment will be placed in later, since this is the only step
	 * where we have all necessary information for that easily available.
	 */
	struct device_tree_node *symbols = dt_find_node_by_path(tree,
		"/__symbols__", NULL, NULL, 0);
	struct device_tree_node *fixups = dt_find_node_by_path(overlay,
		"/__fixups__", NULL, NULL, 0);
	struct device_tree_node *overlay_symbols = dt_find_node_by_path(overlay,
		"/__symbols__", NULL, NULL, 0);
	if (fixups && dt_fixup_all_externals(tree, symbols, overlay,
					     fixups, overlay_symbols) < 0) {
		printk(BIOS_ERR, "cannot match external fixups from overlay\n");
		return -1;
	}

	/* After all this fixing up, we can finally merge overlay into the tree
	   (one fragment at a time, because for some reason it's split up). */
	struct device_tree_node *fragment;
	list_for_each(fragment, overlay->root->children, list_node)
		if (dt_import_fragment(tree, fragment, overlay_symbols) < 0) {
			printk(BIOS_ERR, "bad DT fragment '%s'\n",
			       fragment->name);
			return -1;
		}

	/*
	 * We need to also update /__symbols__ to include labels from this
	 * overlay, in case we want to load further overlays with external
	 * phandle references to it. If the base tree already has a /__symbols__
	 * we merge them together, otherwise we just insert the overlay's
	 * /__symbols__ node into the base tree root.
	 */
	if (overlay_symbols) {
		if (symbols)
			dt_copy_subtree(symbols, overlay_symbols, 0);
		else
			list_insert_after(&overlay_symbols->list_node,
					  &tree->root->children);
	}

	return 0;
}
