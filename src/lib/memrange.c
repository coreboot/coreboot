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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdlib.h>
#include <console/console.h>
#include <memrange.h>

/* Coreboot doesn't have a free() function. Therefore, keep a cache of
 * free'd entries.  */
static LIST_HEAD(free_list);

static inline void range_entry_unlink(struct range_entry *r)
{
	list_del_init(&r->siblings);
}

static struct range_entry *alloc_range(void)
{
	if (!list_empty(&free_list)) {
		struct range_entry *r;

		r = list_first_entry(&free_list, struct range_entry, siblings);
		range_entry_unlink(r);
		return r;
	}
	return malloc(sizeof(struct range_entry));
}

static void range_entry_free(struct range_entry *e)
{
	if (!list_empty(&e->siblings))
		printk(BIOS_ERR, "Freeing linked range_entry!\n");
	list_add(&free_list, &e->siblings);
}

static inline struct range_entry *
range_list_add(struct list_head *head, resource_t begin, resource_t end,
               unsigned long tag)
{
	struct range_entry *new_entry;

	new_entry = alloc_range();
	if (new_entry == NULL) {
		printk(BIOS_ERR, "Could not allocate range_entry!\n");
		return NULL;
	}
	new_entry->begin = begin;
	new_entry->end = end;
	new_entry->tag = tag;
	list_add(&new_entry->siblings, head);

	return new_entry;
}

static void merge_neighbor_entries(struct memranges *ranges)
{
	struct range_entry *cur;
	struct range_entry *prev;
	struct range_entry *tmp;

	prev = NULL;
	/* Merge all neighbors and delete/free the leftover entry. */
	list_for_each_entry_safe(cur, tmp, &ranges->list, siblings) {
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
			range_entry_unlink(cur);
			range_entry_free(cur);
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
	struct range_entry *tmp;

	list_for_each_entry_safe(cur, tmp, &ranges->list, siblings) {
		resource_t tmp_end;

		/* No other ranges are affected. */
		if (end < cur->begin)
			break;

		/* The removal range starts after this one. */
		if (begin > cur->end)
			continue;

		/* The removal range overlaps with the current entry either
		 * partially or fully. However, we need to adjust the removal
		 * range for any holes. */
		if (begin <= cur->begin) {
			begin = cur->begin;

			/* Full removal. */
			if (end >= cur->end) {
				begin = cur->end + 1;
				range_entry_unlink(cur);
				range_entry_free(cur);
				continue;
			}
		}

		/* Clip the end fragment to do proper splitting. */
		tmp_end = end;
		if (end > cur->end)
			tmp_end = cur->end;

		/* Hole punched in middle of entry. */
		if (begin > cur->begin && tmp_end < cur->end) {
			range_list_add(&cur->siblings, end + 1, cur->end,
			               cur->tag);
			cur->end = begin - 1;
			continue;
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
	struct list_head *head;

	head = &ranges->list;

	/* Remove all existing entries covered by the range. */
	remove_memranges(ranges, begin, end, -1);

	/* Find the entry to place the new entry after. Since
	 * remove_memranges() was called above there is a guranteed
	 * spot for this new entry. */
	list_for_each_entry(cur, &ranges->list, siblings) {
		/* Found insertion spot before current entry. */
		if (end < cur->begin)
			break;

		/* Keep track of previous entry to insert new entry after it. */
		head = &cur->siblings;

		/* The new entry starts after this one. */
		if (begin > cur->end)
			continue;

	}

	/* Add new entry and merge with neighbors. */
	range_list_add(head, begin, end, tag);
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

	/* The addresses are aligned to 4096 bytes: the begin address is
	 * aligned down while the end address is aligned up to be conservative
	 * about the full range covered. */
	begin = round_down(base, 4096);
	end = begin + size + (base - begin);
	end = round_up(end, 4096) - 1;
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
};

static void collect_ranges(void *gp, struct device *dev, struct resource *res)
{
	struct collect_context *ctx = gp;

	memranges_insert(ctx->ranges, res->base, res->size, ctx->tag);
}

void memranges_add_resources(struct memranges *ranges,
                                 unsigned long mask, unsigned long match,
                                 unsigned long tag)
{
	struct collect_context context;

	/* Only deal with MEM resources. */
	mask |= IORESOURCE_MEM;
	match |= IORESOURCE_MEM;

	context.ranges = ranges;
	context.tag = tag;
	search_global_resources(mask, match, collect_ranges, &context);
}

void memranges_init(struct memranges *ranges,
                    unsigned long mask, unsigned long match,
                    unsigned long tag)
{
	INIT_LIST_HEAD(&ranges->list);

	memranges_add_resources(ranges, mask, match, tag);
}

void memranges_teardown(struct memranges *ranges)
{
	struct range_entry *cur;
	struct range_entry *tmp;

	/* Merge all neighbors and delete/free the leftover entry. */
	list_for_each_entry_safe(cur, tmp, &ranges->list, siblings) {
		range_entry_unlink(cur);
		range_entry_free(cur);
	}
}

void memranges_fill_holes_up_to(struct memranges *ranges,
                                resource_t limit, unsigned long tag)
{
	struct range_entry *cur;
	struct range_entry *prev;
	struct range_entry *tmp;

	prev = NULL;
	list_for_each_entry_safe(cur, tmp, &ranges->list, siblings) {
		/* First entry. Just set prev. */
		if (prev == NULL) {
			prev = cur;
			continue;
		}

		/* If the previous entry does not directly preceed the current
		 * entry then add a new entry just after the previous one. */
		if (range_entry_end(prev) != cur->begin)
			range_list_add(&prev->siblings, range_entry_end(prev),
			               cur->begin - 1, tag);

		prev = cur;

		/* Hit the requested range limit. No other entries after this
		 * are affected. */
		if (cur->begin >= limit)
			break;
	}

	/* Handle the case where the limit was never reached. A new entry needs
	 * to be added to cover the range up to the limit. */
	if (prev != NULL && range_entry_end(prev) < limit)
		range_list_add(&prev->siblings, range_entry_end(prev),
		               limit - 1, tag);

	/* Merge all entries that were newly added. */
	merge_neighbor_entries(ranges);
}
