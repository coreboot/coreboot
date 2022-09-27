/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MEMRANGE_H_
#define MEMRANGE_H_

#include <device/resource.h>
#include <stdbool.h>
#include <stddef.h>

/* A memranges structure consists of a list of range_entry(s). The structure
 * is exposed so that a memranges can be used on the stack if needed. */
struct memranges {
	struct range_entry *entries;
	/* coreboot doesn't have a free() function. Therefore, keep a cache of
	 * free'd entries.  */
	struct range_entry *free_list;
	/* Alignment(log 2) for base and end addresses of the range. */
	unsigned char align;
};

/* Each region within a memranges structure is represented by a
 * range_entry structure. Use the associated range_entry_(base|end|size|tag)
 * functions to interrogate its properties. i.e. don't rely on one's own
 * interpretation of the fields. */
struct range_entry {
	resource_t begin;
	resource_t end;
	unsigned long tag;
	struct range_entry *next;
};

/* Initialize a range_entry with inclusive beginning address and exclusive
 * end address along with the appropriate tag. */
static inline void range_entry_init(struct range_entry *re,
				resource_t incl_begin, resource_t excl_end,
				unsigned long tag)
{
	re->begin = incl_begin;
	re->end = excl_end - 1;
	re->tag = tag;
	re->next = NULL;
}

/* Return inclusive base address of memory range. */
static inline resource_t range_entry_base(const struct range_entry *r)
{
	return r->begin;
}

/* Return exclusive end address of memory range. */
static inline resource_t range_entry_end(const struct range_entry *r)
{
	return r->end + 1;
}

/* Return size of memory range. */
static inline resource_t range_entry_size(const struct range_entry *r)
{
	return r->end - r->begin + 1;
}

static inline unsigned long range_entry_tag(const struct range_entry *r)
{
	return r->tag;
}

static inline void range_entry_update_tag(struct range_entry *r,
					  unsigned long new_tag)
{
	r->tag = new_tag;
}

static inline bool memranges_is_empty(const struct memranges *ranges)
{
	return ranges->entries == NULL;
}

/* Iterate over each entry in a memranges structure. Ranges cannot
 * be deleted while processing each entry as the list cannot be safely
 * traversed after such an operation.
 * r - range_entry pointer.
 * ranges - memranges pointer */
#define memranges_each_entry(r, ranges) \
	for (r = (ranges)->entries; r != NULL; r = r->next)

/* Initialize memranges structure providing an optional array of range_entry
 * to use as the free list. Additionally, it accepts an align parameter that
 * represents the required alignment(log 2) of addresses. */
void memranges_init_empty_with_alignment(struct memranges *ranges,
					 struct range_entry *free,
					 size_t num_free, unsigned char align);

/* Initialize and fill a memranges structure according to the
 * mask and match type for all memory resources. Tag each entry with the
 * specified type. Additionally, it accepts an align parameter that
 * represents the required alignment(log 2) of addresses. */
void memranges_init_with_alignment(struct memranges *ranges,
		    unsigned long mask, unsigned long match,
		    unsigned long tag, unsigned char align);

/* Initialize memranges structure providing an optional array of range_entry
 * to use as the free list. Addresses are default aligned to 4KiB(2^12). */
#define memranges_init_empty(__ranges, __free, __num_free)	\
	memranges_init_empty_with_alignment(__ranges, __free, __num_free, 12)

/* Initialize and fill a memranges structure according to the
 * mask and match type for all memory resources. Tag each entry with the
 * specified type. Addresses are default aligned to 4KiB(2^12). */
#define memranges_init(__ranges, __mask, __match, __tag)	\
	memranges_init_with_alignment(__ranges, __mask, __match, __tag, 12)

/* Clone a memrange. The new memrange has the same entries as the old one. */
void memranges_clone(struct memranges *newranges, struct memranges *oldranges);

/* Remove and free all entries within the memranges structure. */
void memranges_teardown(struct memranges *ranges);

/* Add memory resources that match with the corresponding mask and match.
 * Each entry will be tagged with the provided tag. e.g.  To populate
 * all cacheable memory resources in the range:
 * memranges_add_resources(range, IORESOURCE_CACHEABLE,
 *                            IORESROUCE_CACHEABLE, my_cacheable_tag); */
void memranges_add_resources(struct memranges *ranges,
			     unsigned long mask, unsigned long match,
			     unsigned long tag);

/* Add memory resources that match with the corresponding mask and match but
 * also provide filter as additional check. The filter will return non-zero
 * to add the resource or zero to not add the resource. Each entry will be
 * tagged with the provided tag. e.g.  To populate all cacheable memory
 * resources in the range with a filter:
 * memranges_add_resources_filter(range, IORESOURCE_CACHEABLE,
 *                         IORESROUCE_CACHEABLE, my_cacheable_tag, filter); */
typedef int (*memrange_filter_t)(struct device *dev, struct resource *res);
void memranges_add_resources_filter(struct memranges *ranges,
				    unsigned long mask, unsigned long match,
				    unsigned long tag,
				    memrange_filter_t filter);

/* Fill all address ranges up to limit (exclusive) not covered by an entry by
 * inserting new entries with the provided tag. */
void memranges_fill_holes_up_to(struct memranges *ranges,
				resource_t limit, unsigned long tag);

/* Create a hole in the range by deleting/modifying entries that overlap with
 * the region specified by base and size. */
void memranges_create_hole(struct memranges *ranges,
			   resource_t base, resource_t size);

/* Insert a resource to the given memranges.  All existing ranges
 * covered by range specified by base and size will be removed before a
 * new one is added. */
void memranges_insert(struct memranges *ranges,
		      resource_t base, resource_t size, unsigned long tag);

/* Update all entries with old_tag to new_tag. */
void memranges_update_tag(struct memranges *ranges, unsigned long old_tag,
			  unsigned long new_tag);

/* Returns next entry after the provided entry. NULL if r is last. */
struct range_entry *memranges_next_entry(struct memranges *ranges,
					 const struct range_entry *r);

/* Steals memory from the available list in given ranges as per the constraints:
 * limit    = Upper bound for the memory range to steal (Inclusive).
 * size     = Requested size for the stolen memory.
 * align    = Required alignment(log 2) for the starting address of the stolen memory.
 * tag      = Use a range that matches the given tag.
 * from_top = Steal the highest possible range.
 *
 * If the constraints can be satisfied, this function creates a hole in the memrange,
 * writes the base address of that hole to stolen_base and returns true. Otherwise it returns
 * false. */
bool memranges_steal(struct memranges *ranges, resource_t limit, resource_t size,
			unsigned char align, unsigned long tag, resource_t *stolen_base,
			bool from_top);

#endif /* MEMRANGE_H_ */
