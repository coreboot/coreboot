/* parser frontend and utility functions for flashmap descriptor language */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef FMD_H_
#define FMD_H_

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define FMD_NOTFOUND UINT_MAX

/**
 * Flags used by flashmap_descriptor.
 * These flags can be set by adding (NAME) after description name.
 * For example, declaring a CBFS section named as COREBOOT for 16k:
 *   COREBOOT(CBFS) 16k
 */
union flashmap_flags {
	struct {
		unsigned int cbfs: 1;  /* The section contains a CBFS area. */
		unsigned int preserve: 1;  /* Preserve the section before update. */
	} f;
	int v;
};

struct flashmap_descriptor {
	char *name;
	bool offset_known;
	/**
	 * Offset relative to that of the parent node.
	 * Exception: for the root node in the descriptor tree, it is optional.
	 * In this case, if absent, it indicates that the flash chip will not be
	 * memory mapped at runtime; otherwise, its value indicates the base
	 * address of the flash chip in the virtual address space rather than
	 * representing an offset into the flash image itself.
	 * It is an error to read this field unless offset_known is set.
	 */
	unsigned offset;
	bool size_known;
	/** It is an error to read this field unless size_known is set. */
	unsigned size;
	size_t list_len;
	union flashmap_flags flags;
	/** It is an error to dereference this array if list_len is 0. */
	struct flashmap_descriptor **list;
};

/**
 * **Client-defined** callback for flag "CBFS".
 * This call is used to notify client code that the user has requested the given
 * section node to be flagged with "CBFS". Implementations of this function
 * should use their return type to tell the compiler whether the flag can be
 * applied and can perform whatever actions are necessary.
 * It's worth reiterating that this is only called on section nodes, and will
 * never be called with the final, complete flashmap_descriptor because
 * it is impossible to set flags for the image as a whole.
 *
 * @param flashmap_descriptor The section node with flag set
 * @return                    Whether this flag can be applied
 */
bool fmd_process_flag_cbfs(const struct flashmap_descriptor *node);

/**
 * Parse and validate a flashmap descriptor from the specified stream.
 * As part of this process, any fields that were omitted in the input are
 * inferred from whatever information is known, if possible. The result is a
 * tree with all its offset and size fields filled, except possibly the former
 * part of the root node in the case of non--memory mapped flash. If a syntax
 * error causes the parser to fail, or if there is not enough information given
 * in the input file to determine any single missing value, the specific error
 * is reported to standard error and this function returns NULL.
 *
 * @param stream File from which to read the (partial) flashmap descriptor
 * @return       Populated flashmap descriptor tree, or NULL on failure
 */
struct flashmap_descriptor *fmd_create(FILE *stream);

/** @param victim Valid descriptor tree to be cleaned up, or NULL for no-op */
void fmd_cleanup(struct flashmap_descriptor *victim);

/**
 * @param tree Must be non-NULL
 * @return     The number of nodes in the tree, including the root
 */
size_t fmd_count_nodes(const struct flashmap_descriptor *tree);

/**
 * @param root The flashmap descriptor to search
 * @param name The name of the sought-after section
 * @return     The desired section node, or NULL if none was found
 */
const struct flashmap_descriptor *fmd_find_node(
		const struct flashmap_descriptor *root, const char *name);

/**
 * @param root Parent node to whose start the "absolute" offset will be relative
 * @param name The name of the node whose offset to determine
 * @return     The "absolute" offset, or FMD_NOTFOUND if the node wasn't found
 */
unsigned fmd_calc_absolute_offset(const struct flashmap_descriptor *root,
							const char *name);

/** @param tree Must be non-NULL */
void fmd_print(const struct flashmap_descriptor *tree);

typedef struct flashmap_descriptor **flashmap_descriptor_iterator_t;

/*
 * Run the subsequent statement once on each descendant of the specified node.
 *
 * @param iterator A flashmap_descriptor_iterator_t (automatically declared)
 * @param parent   The parent node of those over which the loop should iterate
 */
#define fmd_foreach_child_iterator(iterator, parent) \
	for (flashmap_descriptor_iterator_t iterator = parent->list; \
		iterator < parent->list + parent->list_len; ++iterator)

/*
 * Run the subsequent statement once on each descendant of the specified node.
 *
 * @param child  A struct flashmap_descriptor * (automatically declared)
 * @param parent The parent node of those over which the loop should iterate
 */
#define fmd_foreach_child(child, parent) \
	for (struct flashmap_descriptor **fmd_foreach_child_iterator_ = \
						parent->list, *child = NULL; \
						fmd_foreach_child_iterator_ < \
					parent->list + parent->list_len && \
				(child = *fmd_foreach_child_iterator_); \
						++fmd_foreach_child_iterator_)

#endif
