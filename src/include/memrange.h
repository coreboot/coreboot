/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef MEMRANGE_H_
#define MEMRANGE_H_

#include <device/resource.h>
#include <list.h>

/* Each memory_range covers the inclusive addresses of [begin, end]. Even
 * though the fields are exposed in this header file the wrapper functions
 * below should be used for obtaining base, end, and size for forward API
 * compatibility. */
struct memory_range {
	resource_t begin;
	resource_t end;
	struct list_head siblings;
	unsigned long tag;
};

/* A memory_ranges structure consists of a list of memory_range(s). */
struct memory_ranges {
	struct list_head list;
};

/* Return inclusive base address of memory range. */
static inline resource_t memory_range_base(const struct memory_range *r)
{
	return r->begin;
}

/* Return exclusive end address of memory range. */
static inline resource_t memory_range_end(const struct memory_range *r)
{
	return r->end + 1;
}

/* Return size of of memory range. */
static inline resource_t memory_range_size(const struct memory_range *r)
{
	return r->end - r->begin + 1;
}

static inline unsigned long memory_range_tag(const struct memory_range *r)
{
	return r->tag;
}

/* Iterate over each entry in a memory_ranges structure. Ranges cannot
 * be deleted while processing each entry as the list cannot be safely
 * traversed after such an operation.
 * r - memory_range pointer.
 * ranges - memory_rnages pointer */
#define memory_ranges_each_entry(r, ranges) \
	list_for_each_entry(r, &(ranges)->list, siblings)

/* Initialize and fill a memory_ranges structure according to the
 * mask and match type for all memory resources. Tag each entry with the
 * specified type. */
void memory_ranges_init(struct memory_ranges *ranges,
                        unsigned long mask, unsigned long match,
                        unsigned long tag);

/* Remove and free all entries within the memory_ranges structure. */
void memory_ranges_teardown(struct memory_ranges *ranges);

/* Add memory resources that match with the corresponding mask and match.
 * Each entry will be tagged with the provided tag. */
void memory_ranges_add_resources(struct memory_ranges *ranges,
                                 unsigned long mask, unsigned long match,
                                 unsigned long tag);

/* Fill all address ranges not covered by an entry associated with tag. */
void memory_ranges_fill_holes(struct memory_ranges *ranges, unsigned long tag);

/* Delete a resource from the given memory_ranges. */
void delete_memory_range(struct memory_ranges *ranges,
                         resource_t base, resource_t size);

/* Insert a resource to the given memory_ranges.  All existing ranges
 * covered by range specified by base and size will be removed before a
 * new one is added. */
void insert_memory_range(struct memory_ranges *ranges,
                         resource_t base, resource_t size, unsigned long tag);

#endif /* MEMRANGE_H_ */
