/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdlib.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <memrange.h>

static inline void range_entry_link(struct range_entry **prev_ptr,
				    struct range_entry *r)
{
	r->next = *prev_ptr;
	*prev_ptr = r;
}

static inline void range_entry_unlink(struct range_entry **prev_ptr,
				      struct range_entry *r)
{
	*prev_ptr = r->next;
	r->next = NULL;
}

static inline void range_entry_unlink_and_free(struct memranges *ranges,
					       struct range_entry **prev_ptr,
					       struct range_entry *r)
{
	range_entry_unlink(prev_ptr, r);
	range_entry_link(&ranges->free_list, r);
}

static struct range_entry *alloc_range(struct memranges *ranges)
{
	if (ranges->free_list != NULL) {
		struct range_entry *r;

		r = ranges->free_list;
		range_entry_unlink(&ranges->free_list, r);
		return r;
	}
	if (ENV_PAYLOAD_LOADER)
		return malloc(sizeof(struct range_entry));
	return NULL;
}

static inline struct range_entry *
range_list_add(struct memranges *ranges, struct range_entry **prev_ptr,
	       resource_t begin, resource_t end, unsigned long tag)
{
	struct range_entry *new_entry;

	new_entry = alloc_range(ranges);
	if (new_entry == NULL) {
		printk(BIOS_ERR, "Could not allocate range_entry!\n");
		return NULL;
	}
	new_entry->begin = begin;
	new_entry->end = end;
	new_entry->tag = tag;
	range_entry_link(prev_ptr, new_entry);

	return new_entry;
}

static void merge_neighbor_entries(struct memranges *ranges)
{
	struct range_entry *cur;
	struct range_entry *prev;

	prev = NULL;
	/* Merge all neighbors and delete/free the leftover entry. */
	for (cur = ranges->entries; cur != NULL; cur = cur->next) {
		/* First entry. Just set prev. */
		if (prev == NULL) {
			prev = cur;
			continue;
		}

		/* If the previous entry merges with the current update the
		 * previous entry to cover full range and delete current from
		 * the list. */
		if (prev->end + 1 >= cur->begin && prev->tag == cur->tag) {
			prev->end = cur->end;
			range_entry_unlink_and_free(ranges, &prev->next, cur);
			/* Set cur to prev so cur->next is valid since cur
			 * was just unlinked and free. */
			cur = prev;
			continue;
		}

		prev = cur;
	}
}

static void remove_memranges(struct memranges *ranges,
			     resource_t begin, resource_t end,
			     unsigned long unused)
{
	struct range_entry *cur;
	struct range_entry *next;
	struct range_entry **prev_ptr;

	prev_ptr = &ranges->entries;
	for (cur = ranges->entries; cur != NULL; cur = next) {
		resource_t tmp_end;

		/* Cache the next value to handle unlinks. */
		next = cur->next;

		/* No other ranges are affected. */
		if (end < cur->begin)
			break;

		/* The removal range starts after this one. */
		if (begin > cur->end) {
			prev_ptr = &cur->next;
			continue;
		}

		/* The removal range overlaps with the current entry either
		 * partially or fully. However, we need to adjust the removal
		 * range for any holes. */
		if (begin <= cur->begin) {
			begin = cur->begin;

			/* Full removal. */
			if (end >= cur->end) {
				begin = cur->end + 1;
				range_entry_unlink_and_free(ranges, prev_ptr,
							    cur);
				continue;
			}
		}

		/* prev_ptr can be set now that the unlink path wasn't taken. */
		prev_ptr = &cur->next;

		/* Clip the end fragment to do proper splitting. */
		tmp_end = end;
		if (end > cur->end)
			tmp_end = cur->end;

		/* Hole punched in middle of entry. */
		if (begin > cur->begin && tmp_end < cur->end) {
			range_list_add(ranges, &cur->next, end + 1, cur->end,
				       cur->tag);
			cur->end = begin - 1;
			break;
		}

		/* Removal at beginning. */
		if (begin == cur->begin)
			cur->begin = tmp_end + 1;

		/* Removal at end. */
		if (tmp_end == cur->end)
			cur->end = begin - 1;
	}
}

static void merge_add_memranges(struct memranges *ranges,
				resource_t begin, resource_t end,
				unsigned long tag)
{
	struct range_entry *cur;
	struct range_entry **prev_ptr;

	prev_ptr = &ranges->entries;

	/* Remove all existing entries covered by the range. */
	remove_memranges(ranges, begin, end, -1);

	/* Find the entry to place the new entry after. Since
	 * remove_memranges() was called above there is a guaranteed
	 * spot for this new entry. */
	for (cur = ranges->entries; cur != NULL; cur = cur->next) {
		/* Found insertion spot before current entry. */
		if (end < cur->begin)
			break;

		/* Keep track of previous entry to insert new entry after it. */
		prev_ptr = &cur->next;

		/* The new entry starts after this one. */
		if (begin > cur->end)
			continue;

	}

	/* Add new entry and merge with neighbors. */
	range_list_add(ranges, prev_ptr, begin, end, tag);
	merge_neighbor_entries(ranges);
}

void memranges_update_tag(struct memranges *ranges, unsigned long old_tag,
			  unsigned long new_tag)
{
	struct range_entry *r;

	memranges_each_entry(r, ranges) {
		if (range_entry_tag(r) == old_tag)
			range_entry_update_tag(r, new_tag);
	}

	merge_neighbor_entries(ranges);
}

typedef void (*range_action_t)(struct memranges *ranges,
			       resource_t begin, resource_t end,
			       unsigned long tag);

static void do_action(struct memranges *ranges,
		      resource_t base, resource_t size, unsigned long tag,
		       range_action_t action)
{
	resource_t end;
	resource_t begin;

	if (size == 0)
		return;

	/* The addresses are aligned to (1ULL << ranges->align): the begin address is
	 * aligned down while the end address is aligned up to be conservative
	 * about the full range covered. */
	begin = ALIGN_DOWN(base, POWER_OF_2(ranges->align));
	end = begin + size + (base - begin);
	end = ALIGN_UP(end, POWER_OF_2(ranges->align)) - 1;
	action(ranges, begin, end, tag);
}

void memranges_create_hole(struct memranges *ranges,
			   resource_t base, resource_t size)
{
	do_action(ranges, base, size, -1, remove_memranges);
}

void memranges_insert(struct memranges *ranges,
		      resource_t base, resource_t size, unsigned long tag)
{
	do_action(ranges, base, size, tag, merge_add_memranges);
}

struct collect_context {
	struct memranges *ranges;
	unsigned long tag;
	memrange_filter_t filter;
};

static void collect_ranges(void *gp, struct device *dev, struct resource *res)
{
	struct collect_context *ctx = gp;

	if (res->size == 0)
		return;

	if (ctx->filter == NULL || ctx->filter(dev, res))
		memranges_insert(ctx->ranges, res->base, res->size, ctx->tag);
}

void memranges_add_resources_filter(struct memranges *ranges,
				    unsigned long mask, unsigned long match,
				    unsigned long tag,
				    memrange_filter_t filter)
{
	struct collect_context context;

	/* Only deal with MEM resources. */
	mask |= IORESOURCE_MEM;
	match |= IORESOURCE_MEM;

	context.ranges = ranges;
	context.tag = tag;
	context.filter = filter;
	search_global_resources(mask, match, collect_ranges, &context);
}

void memranges_add_resources(struct memranges *ranges,
			     unsigned long mask, unsigned long match,
			     unsigned long tag)
{
	memranges_add_resources_filter(ranges, mask, match, tag, NULL);
}

void memranges_init_empty_with_alignment(struct memranges *ranges,
					 struct range_entry *to_free,
					 size_t num_free, unsigned char align)
{
	size_t i;

	ranges->entries = NULL;
	ranges->free_list = NULL;
	ranges->align = align;

	for (i = 0; i < num_free; i++)
		range_entry_link(&ranges->free_list, &to_free[i]);
}

void memranges_init_with_alignment(struct memranges *ranges,
				   unsigned long mask, unsigned long match,
				   unsigned long tag, unsigned char align)
{
	memranges_init_empty_with_alignment(ranges, NULL, 0, align);
	memranges_add_resources(ranges, mask, match, tag);
}

/* Clone a memrange. The new memrange has the same entries as the old one. */
void memranges_clone(struct memranges *newranges, struct memranges *oldranges)
{
	struct range_entry *r, *cur;
	struct range_entry **prev_ptr;

	memranges_init_empty_with_alignment(newranges, NULL, 0, oldranges->align);

	prev_ptr = &newranges->entries;
	memranges_each_entry(r, oldranges) {
		cur = range_list_add(newranges, prev_ptr, r->begin, r->end,
				     r->tag);
		prev_ptr = &cur->next;
	}
}

void memranges_teardown(struct memranges *ranges)
{
	while (ranges->entries != NULL) {
		range_entry_unlink_and_free(ranges, &ranges->entries,
					    ranges->entries);
	}
}

void memranges_fill_holes_up_to(struct memranges *ranges,
				resource_t limit, unsigned long tag)
{
	struct range_entry *cur;
	struct range_entry *prev;

	prev = NULL;
	for (cur = ranges->entries; cur != NULL; cur = cur->next) {
		/* First entry. Just set prev. */
		if (prev == NULL) {
			prev = cur;
			continue;
		}

		/* If the previous entry does not directly precede the current
		 * entry then add a new entry just after the previous one. */
		if (range_entry_end(prev) != cur->begin) {
			resource_t end;

			end = cur->begin - 1;
			if (end >= limit)
				end = limit - 1;
			range_list_add(ranges, &prev->next,
				       range_entry_end(prev), end, tag);
		}

		prev = cur;

		/* Hit the requested range limit. No other entries after this
		 * are affected. */
		if (cur->begin >= limit)
			break;
	}

	/* Handle the case where the limit was never reached. A new entry needs
	 * to be added to cover the range up to the limit. */
	if (prev != NULL && range_entry_end(prev) < limit)
		range_list_add(ranges, &prev->next, range_entry_end(prev),
			       limit - 1, tag);

	/* Merge all entries that were newly added. */
	merge_neighbor_entries(ranges);
}

struct range_entry *memranges_next_entry(struct memranges *ranges,
					 const struct range_entry *r)
{
	return r->next;
}

/* Find a range entry that satisfies the given constraints to fit a hole that matches the
 * required alignment, is big enough, does not exceed the limit and has a matching tag. */
static const struct range_entry *
memranges_find_entry(struct memranges *ranges, resource_t limit, resource_t size,
		     unsigned char align, unsigned long tag, bool last)
{
	const struct range_entry *r, *last_entry = NULL;
	resource_t base, end;

	if (size == 0)
		return NULL;

	memranges_each_entry(r, ranges) {

		if (r->tag != tag)
			continue;

		base = ALIGN_UP(r->begin, POWER_OF_2(align));
		end = base + size - 1;

		if (end > r->end)
			continue;

		/*
		 * If end for the hole in the current range entry goes beyond the requested
		 * limit, then none of the following ranges can satisfy this request because all
		 * range entries are maintained in increasing order.
		 */
		if (end > limit)
			break;

		if (!last)
			return r;

		last_entry = r;
	}

	return last_entry;
}

bool memranges_steal(struct memranges *ranges, resource_t limit, resource_t size,
			unsigned char align, unsigned long tag, resource_t *stolen_base,
			bool from_top)
{
	const struct range_entry *r;

	r = memranges_find_entry(ranges, limit, size, align, tag, from_top);
	if (r == NULL)
		return false;

	if (from_top) {
		/* Ensure we're within the range, even aligned down.
		   Proof is simple: If ALIGN_UP(r->begin) would be
		   higher, the stolen range wouldn't fit.*/
		assert(r->begin <= ALIGN_DOWN(range_entry_end(r) - size, POWER_OF_2(align)));
		*stolen_base = ALIGN_DOWN(range_entry_end(r) - size, POWER_OF_2(align));
	} else {
		*stolen_base = ALIGN_UP(r->begin, POWER_OF_2(align));
	}
	memranges_create_hole(ranges, *stolen_base, size);

	return true;
}
