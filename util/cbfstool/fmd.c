/*
 * fmd.c, parser frontend and utility functions for flashmap descriptor language
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

#include "fmd.h"

#include "common.h"
#include "fmd_parser.h"
#include "fmd_scanner.h"
#include "option.h"

#include <assert.h>
#include <search.h>
#include <string.h>

/*
 * Validate the given flashmap descriptor node's properties. In particular:
 *  - Ensure its name is globally unique.
 *  - Ensure its offset, if known, isn't located before the end of the previous
 *    section, if this can be determined.
 *  - Ensure its offset, if known, isn't located after the beginning of the next
 *    section or off the end of its parent section, if this can be determined.
 *  - Ensure its size is nonzero.
 *  - Ensure that the combination of its size and offset, if they are both
 *    known, doesn't place its end after the beginning of the next section or
 *    off the end of its parent section, if this can be determined.
 * In the case of a validation error, the particular problem is reported to
 * standard error and this function returns false. It should be noted that this
 * function makes no claim that the members of the node's child list are valid:
 * under no circumstances is any recursive validation performed.
 *
 * @param node  The flashmap descriptor node to be validated
 * @param start Optional minimum permissible base of the section to be
 *              validated, to be provided if known
 * @param end   Optional maximum permissible offset to the end of the section to
 *              be validated, to be provided if known
 * @return      Whether the node is valid
 */
static bool validate_descriptor_node(const struct flashmap_descriptor *node,
		struct unsigned_option start, struct unsigned_option end)
{
	assert(node);

#if __GLIBC__
	/* GLIBC is different than the BSD libc implementations:
	 *   The  hdestroy() [function does] not free the buffers pointed
	 *   to by the key and data elements of the hash table entries.
	 * vs:
	 *   The hdestroy() function calls free(3) for each comparison key in
	 *   the search table but not the data item associated with the key.
	 */
	ENTRY search_key = {node->name, NULL};
#else
	ENTRY search_key = {strdup(node->name), NULL};
#endif

	if (hsearch(search_key, FIND)) {
		ERROR("Multiple sections with name '%s'\n", node->name);
		return false;
	}
	if (!hsearch(search_key, ENTER))
		assert(false);

	if (node->offset_known) {
		if (start.val_known && node->offset < start.val) {
			ERROR("Section '%s' starts too low\n", node->name);
			return false;
		} else if (end.val_known && node->offset > end.val) {
			ERROR("Section '%s' starts too high\n", node->name);
			return false;
		}
	}

	if (node->size_known) {
		if (node->size == 0) {
			ERROR("Section '%s' given no space\n", node->name);
			return false;
		} else if (node->offset_known) {
			unsigned node_end = node->offset + node->size;
			if (end.val_known && node_end > end.val) {
				ERROR("Section '%s' too big\n", node->name);
				return false;
			}
		}
	}

	return true;
}

/*
 * Performs reverse lateral processing of sibling nodes, as described by the
 * documentation of its caller, validate_and_complete_info(). If it encounters
 * a node that is invalid in a way that couldn't have been discovered earlier,
 * it explains the problem to standard output and returns false.
 *
 * @param first_incomplete_it First node whose offset or size couldn't be
 *                            determined during forward processing
 * @param cur_incomplete_it   Last node whose offset or size is unknown
 * @param end_watermark       Offset to the end of the unresolved region
 * @return                    Whether all completed nodes were still valid
 */
static bool complete_missing_info_backward(
			flashmap_descriptor_iterator_t first_incomplete_it,
			flashmap_descriptor_iterator_t cur_incomplete_it,
							unsigned end_watermark)
{
	assert(first_incomplete_it);
	assert(cur_incomplete_it);
	assert(cur_incomplete_it >= first_incomplete_it);

	do {
		struct flashmap_descriptor *cur = *cur_incomplete_it;

		assert(cur->offset_known || cur->size_known);
		if (!cur->offset_known) {
			if (cur->size > end_watermark) {
				ERROR("Section '%s' too big\n", cur->name);
				return false;
			}
			cur->offset_known = true;
			cur->offset = end_watermark -= cur->size;
		} else if (!cur->size_known) {
			if (cur->offset > end_watermark) {
				ERROR("Section '%s' starts too high\n",
								cur->name);
				return false;
			}
			cur->size_known = true;
			cur->size = end_watermark - cur->offset;
			end_watermark = cur->offset;
		}
	} while (--cur_incomplete_it >= first_incomplete_it);

	return true;
}

/*
 * Recursively examine each descendant of the provided flashmap descriptor node
 * to ensure its position and size are known, attempt to infer them otherwise,
 * and validate their values once they've been populated.
 * This processes nodes according to the following algorithm:
 *  - At each level of the tree, it moves laterally between siblings, keeping
 *    a watermark of its current offset relative to the previous section, which
 *    it uses to fill in any unknown offsets it encounters along the way.
 *  - The first time it encounters a sibling with unknown size, it loses track
 *    of the watermark, and is therefore unable to complete further offsets;
 *    instead, if the watermark was known before, it marks the current node as
 *    the first that couldn't be completed in the initial pass.
 *  - If the current watermark is unknown (i.e. a node has been marked as the
 *    first incomplete one) and one with a fixed offset is encountered, a
 *    reverse lateral traversal is dispatched that uses that provided offset as
 *    a reverse watermark to complete all unknown fields until it finishes with
 *    the node marked as the first incomplete one: at this point, that flag is
 *    cleared, the watermark is updated, and forward processing resumes from
 *    where it left off.
 *  - If the watermark is unknown (i.e. node(s) are incomplete) after traversing
 *    all children of a particular parent node, reverse processing is employed
 *    as described above, except that the reverse watermark is initialized to
 *    the parent node's size instead of the (nonexistent) next node's offset.
 *  - Once all of a node's children have been processed, the algorithm applies
 *    itself recursively to each of the child nodes; thus, lower levels of the
 *    tree are processed only after their containing levels are finished.
 * This approach can fail in two possible ways (in which case the problem is
 * reported to standard output and this function returns false):
 *  - Processing reveals that some node's provided value is invalid in some way.
 *  - Processing determines that one or more provided values require an omitted
 *    field to take a nonsensical value.
 *  - Processing determines that it is impossible to determine a group of
 *    omitted values. This state is detected when a node whose offset *and*
 *    value are omitted is encountered during forward processing and while the
 *    current watermark is unknown: in such a case, neither can be known without
 *    being provided with either the other or more context.
 * The function notably performs neither validation nor completion on the parent
 * node it is passed; thus, it is important to ensure that that node is valid.
 * (At the very least, it must have a valid size field in order for the
 * algorithm to work on its children.)
 *
 * @param cur_level Parent node, which must minimally already have a valid size
 * @return          Whether completing and validating the children succeeded
 */
static bool validate_and_complete_info(struct flashmap_descriptor *cur_level)
{
	assert(cur_level);
	assert(cur_level->size_known);

	// Our watermark is only known when first_incomplete_it is NULL.
	flashmap_descriptor_iterator_t first_incomplete_it = NULL;
	unsigned watermark = 0;

	fmd_foreach_child_iterator(cur_it, cur_level) {
		struct flashmap_descriptor *cur_section = *cur_it;

		if (first_incomplete_it) {
			if (cur_section->offset_known) {
				if (complete_missing_info_backward(
						first_incomplete_it, cur_it - 1,
							cur_section->offset)) {
					first_incomplete_it = NULL;
					watermark = cur_section->offset;
				} else {
					return false;
				}
			}
			// Otherwise, we can't go back until a provided offset.
		} else if (!cur_section->offset_known) {
			cur_section->offset_known = true;
			cur_section->offset = watermark;
		}

		assert(cur_level->size_known);
		struct unsigned_option max_endpoint = {true, cur_level->size};
		if (cur_it != cur_level->list + cur_level->list_len - 1) {
			struct flashmap_descriptor *next_section = cur_it[1];
			max_endpoint.val_known = next_section->offset_known;
			max_endpoint.val = next_section->offset;
		}
		if (!validate_descriptor_node(cur_section,
							(struct unsigned_option)
					{!first_incomplete_it, watermark},
								max_endpoint))
			return false;

		if (!cur_section->size_known) {
			if (!cur_section->offset_known) {
				ERROR("Cannot determine either offset or size of section '%s'\n",
							cur_section->name);
				return false;
			} else if (!first_incomplete_it) {
				first_incomplete_it = cur_it;
			} else {
				// We shouldn't find an unknown size within an
				// incomplete region because the backward
				// traversal at the beginning of this node's
				// processing should have concluded said region.
				assert(!first_incomplete_it);
			}
		} else if (!first_incomplete_it) {
			watermark = cur_section->offset + cur_section->size;
		}
	}

	if (first_incomplete_it &&
			!complete_missing_info_backward(first_incomplete_it,
				cur_level->list + cur_level->list_len - 1,
							cur_level->size))
		return false;

	fmd_foreach_child(cur_section, cur_level) {
		assert(cur_section->offset_known);
		assert(cur_section->size_known);

		if (!validate_and_complete_info(cur_section))
			return false;
	}

	return true;
}

static void print_with_prefix(const struct flashmap_descriptor *tree,
								const char *pre)
{
	assert(tree);
	assert(pre);

	printf("%ssection '%s' has ", pre, tree->name);

	if (tree->offset_known)
		printf("offset %uB, ", tree->offset);
	else
		fputs("unknown offset, ", stdout);

	if (tree->size_known)
		printf("size %uB, ", tree->size);
	else
		fputs("unknown size, ", stdout);

	printf("and %zu subsections", tree->list_len);
	if (tree->list_len) {
		puts(":");

		char child_prefix[strlen(pre) + 1];
		strcpy(child_prefix, pre);
		strcat(child_prefix, "\t");
		fmd_foreach_child(each, tree)
			print_with_prefix(each, child_prefix);
	} else {
		puts("");
	}
}

struct flashmap_descriptor *fmd_create(FILE *stream)
{
	assert(stream);

	yyin = stream;

	struct flashmap_descriptor *ret = NULL;
	if (yyparse() == 0)
		ret = res;

	yylex_destroy();
	yyin = NULL;
	res = NULL;

	if (ret) {
		// This hash table is used to store the declared name of each
		// section and ensure that each is globally unique.
		if (!hcreate(fmd_count_nodes(ret))) {
			perror("E: While initializing hashtable");
			fmd_cleanup(ret);
			return NULL;
		}

		// Even though we haven't checked that the root node (ret) has
		// a size field as required by this function, the parser
		// warrants that it does because the grammar requires it.
		if (!validate_and_complete_info(ret)) {
			hdestroy();
			fmd_cleanup(ret);
			return NULL;
		}

		hdestroy();
	}

	return ret;
}

void fmd_cleanup(struct flashmap_descriptor *victim)
{
	if (!victim)
		return;

	free(victim->name);
	for (unsigned idx = 0; idx < victim->list_len; ++idx)
		fmd_cleanup(victim->list[idx]);
	free(victim->list);
	free(victim);
}

size_t fmd_count_nodes(const struct flashmap_descriptor *tree)
{
	assert(tree);

	if (!tree->list_len)
		return 1;

	unsigned count = 1;
	fmd_foreach_child(lower, tree)
		count += fmd_count_nodes(lower);
	return count;
}

const struct flashmap_descriptor *fmd_find_node(
		const struct flashmap_descriptor *root, const char *name)
{
	assert(root);
	assert(name);

	if (strcmp(root->name, name) == 0)
		return root;

	fmd_foreach_child(descendant, root) {
		const struct flashmap_descriptor *match =
						fmd_find_node(descendant, name);
		if (match)
			return match;
	}
	return NULL;
}

unsigned fmd_calc_absolute_offset(const struct flashmap_descriptor *root,
							const char *name)
{
	assert(root);
	assert(name);

	if (strcmp(root->name, name) == 0)
		return 0;

	fmd_foreach_child(descendant, root) {
		unsigned subtotal = fmd_calc_absolute_offset(descendant, name);
		if (subtotal != FMD_NOTFOUND)
			return descendant->offset + subtotal;
	}
	return FMD_NOTFOUND;
}

void fmd_print(const struct flashmap_descriptor *tree)
{
	print_with_prefix(tree, "");
}
