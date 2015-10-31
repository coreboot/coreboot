/*
 * fmd.h, parser frontend and utility functions for flashmap descriptor language
 *
 * Copyright (C) 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef FMD_H_
#define FMD_H_

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define FMD_NOTFOUND UINT_MAX

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
	/** It is an error to dereference this array if list_len is 0. */
	struct flashmap_descriptor **list;
};

/**
 * **Client-defined** callback.
 * This call is used to notify client code that the user has annotated the given
 * section node by accompanying it with a string enclosed in parentheses. It is
 * only invoked for nodes that have annotations, and then only once per node.
 * The annotations' syntactic validity and semantic meaning are not determined
 * by the compiler; rather, implementations of this function should use their
 * return type to tell the compiler whether the annotation was valid syntax, as
 * well as perform whatever actions are necessary given the particular
 * annotation. It's worth reiterating that this is only called on section nodes,
 * and will never be called with the final, complete flashmap_descriptor because
 * it is impossible to annotate the image as a whole. Note that, although the
 * node received by this function will be preserved in memory as part of the
 * ultimate flashmap_descriptor, the annotation string will only persist during
 * this function call: if the implementation needs it longer, it must copy it.
 *
 * @param flashmap_descriptor The section node carrying the annotation
 * @param annotation          What the user wrote (only valid during callback)
 * @return                    Whether this annotation represented valid syntax
 */
bool fmd_process_annotation_impl(const struct flashmap_descriptor *node,
							const char *annotation);

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
