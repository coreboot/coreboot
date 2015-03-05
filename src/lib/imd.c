/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google, Inc.
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
 * Foundation, Inc.
 */

#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <imd.h>
#include <stdlib.h>
#include <string.h>

/* For more details on implementation and usage please see the imd.h header. */

static const uint32_t IMD_ROOT_PTR_MAGIC = 0xc0389481;
static const uint32_t IMD_ENTRY_MAGIC = ~0xc0389481;
static const size_t LIMIT_ALIGN = 4096;

/* In-memory data structures. */
struct imd_root_pointer {
	uint32_t magic;
	/* Relative to upper limit/offset. */
	int32_t root_offset;
} __attribute__((packed));

struct imd_entry {
	uint32_t magic;
	/* start is located relative to imd_root */
	int32_t start_offset;
	uint32_t size;
	uint32_t id;
} __attribute__((packed));

struct imd_root {
	uint32_t max_entries;
	uint32_t num_entries;
	uint32_t flags;
	uint32_t entry_align;
	/* Used for fixing the size of an imd. Relative to the root. */
	int32_t max_offset;
	struct imd_entry entries[0];
} __attribute__((packed));

#define IMD_FLAG_LOCKED 1

static void *relative_pointer(void *base, ssize_t offset)
{
	intptr_t b = (intptr_t)base;
	b += offset;
	return (void *)b;
}

static bool imd_root_pointer_valid(const struct imd_root_pointer *rp)
{
	return !!(rp->magic == IMD_ROOT_PTR_MAGIC);
}

static struct imd_root *imd_root(const struct imd *imd)
{
	return imd->r;
}

/*
 * The root pointer is relative to the upper limit of the imd. i.e. It sits
 * just below the upper limit.
 */
static struct imd_root_pointer *imd_get_root_pointer(const struct imd *imd)
{
	struct imd_root_pointer *rp;

	rp = relative_pointer((void *)imd->limit, -sizeof(*rp));

	return rp;
}

static void imd_link_root(struct imd_root_pointer *rp, struct imd_root *r)
{
	rp->magic = IMD_ROOT_PTR_MAGIC;
	rp->root_offset = (int32_t)((intptr_t)r - (intptr_t)rp);
}

static void imd_entry_assign(struct imd_entry *e, uint32_t id,
				ssize_t offset, size_t size)
{
	e->magic = IMD_ENTRY_MAGIC;
	e->start_offset = offset;
	e->size = size;
	e->id = id;
}

static bool root_is_locked(const struct imd_root *r)
{
	return !!(r->flags & IMD_FLAG_LOCKED);
}

static struct imd_entry *root_last_entry(struct imd_root *r)
{
	return &r->entries[r->num_entries - 1];
}

/* Initialize imd handle. */
void imd_handle_init(struct imd *imd, void *upper_limit)
{
	uintptr_t limit = (uintptr_t)upper_limit;
	/* Upper limit is aligned down to 4KiB */
	imd->limit = ALIGN_DOWN(limit, LIMIT_ALIGN);
	imd->r = NULL;
}

void imd_handle_init_partial_recovery(struct imd *imd)
{
	struct imd_root_pointer *rp;

	imd_handle_init(imd, (void *)imd->limit);

	rp = imd_get_root_pointer(imd);
	imd->r = relative_pointer(rp, rp->root_offset);
}

int imd_create_empty(struct imd *imd, size_t root_size, size_t entry_align)
{
	struct imd_root_pointer *rp;
	struct imd_root *r;
	struct imd_entry *e;
	ssize_t root_offset;
	size_t entries_size;

	if (!imd->limit)
		return -1;

	/* root_size and entry_align should be a power of 2. */
	assert(IS_POWER_OF_2(root_size));
	assert(IS_POWER_OF_2(entry_align));

	/*
	 * root_size needs to be large enough to accomodate root pointer and
	 * root book keeping structure. The caller needs to ensure there's
	 * enough room for tracking individual allocations.
	 */
	if (root_size < (sizeof(*rp) + sizeof(*r)))
		return -1;

	/* For simplicity don't allow sizes or alignments to exceed LIMIT_ALIGN. */
	if (root_size > LIMIT_ALIGN || entry_align > LIMIT_ALIGN)
		return -1;

	/* Additionally, don't handle an entry alignment > root_size. */
	if (entry_align > root_size)
		return -1;

	rp = imd_get_root_pointer(imd);

	root_offset = -(ssize_t)root_size;
	/* Set root pointer. */
	imd->r = relative_pointer((void *)imd->limit, root_offset);
	r = imd_root(imd);
	imd_link_root(rp, r);

	memset(r, 0, sizeof(*r));
	r->entry_align = entry_align;

	/* Calculate size left for entries. */
	entries_size = root_size;
	entries_size -= sizeof(*rp);
	entries_size -= sizeof(*r);

	r->max_entries = entries_size / sizeof(r->entries[0]);

	/* Fill in first entry covering the root region. */
	r->num_entries = 1;
	e = &r->entries[0];
	imd_entry_assign(e, CBMEM_ID_IMD_ROOT, 0, root_size);

	printk(BIOS_DEBUG, "IMD: root @ %p %u entries.\n", r, r->max_entries);

	return 0;
}

int imd_limit_size(struct imd *imd, size_t max_size)
{
	struct imd_root *r;
	ssize_t smax_size;
	size_t root_size;

	r = imd_root(imd);
	if (r == NULL)
		return -1;

	root_size = imd->limit - (uintptr_t)r;

	if (max_size < root_size)
		return -1;

	/* Take into account the root size. */
	smax_size = max_size - root_size;
	smax_size = -smax_size;

	r->max_offset = smax_size;

	return 0;
}

int imd_recover(struct imd *imd)
{
	struct imd_root_pointer *rp;
	struct imd_root *r;
	uintptr_t low_limit;
	size_t i;

	if (!imd->limit);
		return -1;

	rp = imd_get_root_pointer(imd);

	if (!imd_root_pointer_valid(rp))
		return -1;

	r = relative_pointer(rp, rp->root_offset);

	/* Confirm the root and root pointer are just under the limit. */
	if (ALIGN_UP((uintptr_t)&r->entries[r->max_entries], LIMIT_ALIGN) !=
			imd->limit)
		return -1;

	if (r->num_entries > r->max_entries)
		return -1;

	/* Entry alignment should be power of 2. */
	if (!IS_POWER_OF_2(r->entry_align))
		return -1;

	low_limit = (uintptr_t)relative_pointer(r, r->max_offset);

	/* If no max_offset then lowest limit is 0. */
	if (low_limit == (uintptr_t)r)
		low_limit = 0;

	for (i = 0; i < r->num_entries; i++) {
		uintptr_t start_addr;
		const struct imd_entry *e = &r->entries[i];

		if (e->magic != IMD_ENTRY_MAGIC)
			return -1;

		start_addr = (uintptr_t)relative_pointer(r, e->start_offset);
		if (start_addr  < low_limit)
			return -1;
		if (start_addr >= imd->limit ||
				(start_addr + e->size) > imd->limit)
			return -1;
	}

	/* Set root pointer. */
	imd->r = r;

	return 0;
}

int imd_lockdown(struct imd *imd)
{
	struct imd_root *r;

	r = imd_root(imd);
	if (r == NULL)
		return -1;

	r->flags |= IMD_FLAG_LOCKED;

	return 0;
}

int imd_region_used(struct imd *imd, void **base, size_t *size)
{
	struct imd_root *r;
	struct imd_entry *e;
	void *low_addr;
	size_t sz_used;

	if (!imd->limit)
		return -1;

	r = imd_root(imd);

	if (r == NULL)
		return -1;

	/* Use last entry to obtain lowest address. */
	e = root_last_entry(r);

	low_addr = relative_pointer(r, e->start_offset);

	/* Total size used is the last entry's base up to the limit. */
	sz_used = imd->limit - (uintptr_t)low_addr;

	*base = low_addr;
	*size = sz_used;

	return 0;
}

static struct imd_entry *imd_entry_add_to_root(struct imd_root *r, uint32_t id,
						size_t size)
{
	struct imd_entry *entry;
	struct imd_entry *last_entry;
	ssize_t e_offset;
	size_t used_size;

	if (r->num_entries == r->max_entries)
		return NULL;

	/* Determine total size taken up by entry. */
	used_size = ALIGN_UP(size, r->entry_align);

	last_entry = root_last_entry(r);

	/* See if size overflows imd total size. */
	if (r->max_offset != 0) {
		size_t remaining = last_entry->start_offset - r->max_offset;

		if (used_size > remaining)
			return NULL;
	}

	/*
	 * Determine if offset field overflows. All offsets should be lower
	 * than the previous one.
	 */
	e_offset = last_entry->start_offset;
	e_offset -= (ssize_t)used_size;
	if (e_offset > last_entry->start_offset)
		return NULL;

	entry = root_last_entry(r) + 1;
	r->num_entries++;

	imd_entry_assign(entry, id, e_offset, size);

	return entry;
}

const struct imd_entry *imd_entry_add(const struct imd *imd, uint32_t id,
					size_t size)
{
	struct imd_root *r;

	r = imd_root(imd);

	if (r == NULL)
		return NULL;

	if (root_is_locked(r))
		return NULL;

	return imd_entry_add_to_root(r, id, size);
}

const struct imd_entry *imd_entry_find(const struct imd *imd, uint32_t id)
{
	struct imd_root *r;
	struct imd_entry *e;
	size_t i;

	r = imd_root(imd);

	if (r == NULL)
		return NULL;

	e = NULL;
	/* Skip first entry covering the root. */
	for (i = 1; i < r->num_entries; i++) {
		if (id == r->entries[i].id) {
			e = &r->entries[i];
			break;
		}
	}

	return e;
}

const struct imd_entry *imd_entry_find_or_add(const struct imd *imd,
						uint32_t id, size_t size)
{
	const struct imd_entry *e;

	e = imd_entry_find(imd, id);

	if (e != NULL)
		return e;

	return imd_entry_add(imd, id, size);
}

size_t imd_entry_size(const struct imd *imd, const struct imd_entry *entry)
{
	return entry->size;
}

void *imd_entry_at(const struct imd *imd, const struct imd_entry *entry)
{
	struct imd_root *r;

	r = imd_root(imd);

	if (r == NULL)
		return NULL;

	return relative_pointer(r, entry->start_offset);
}

int imd_entry_remove(const struct imd *imd, const struct imd_entry *entry)
{
	struct imd_root *r;

	r = imd_root(imd);

	if (r == NULL)
		return -1;

	if (root_is_locked(r))
		return -1;

	if (entry != root_last_entry(r))
		return -1;

	r->num_entries--;

	return 0;
}

int imd_print_entries(const struct imd *imd, const struct imd_lookup *lookup,
			size_t size)
{
	struct imd_root *r;
	size_t i;
	size_t j;

	if (imd == NULL)
		return -1;

	r = imd_root(imd);

	if (r == NULL)
		return -1;

	for (i = 0; i < r->num_entries; i++) {
		const char *name = NULL;
		const struct imd_entry *e = &r->entries[i];

		for (j = 0; j < size; j++) {
			if (lookup[j].id == e->id) {
				name = lookup[j].name;
				break;
			}
		}

		if (name == NULL)
			printk(BIOS_DEBUG, "%08x   ", e->id);
		else
			printk(BIOS_DEBUG, "%s", name);
		printk(BIOS_DEBUG, "%2zu. ", i);
		printk(BIOS_DEBUG, "%p ", imd_entry_at(imd, e));
		printk(BIOS_DEBUG, "%08zx\n", imd_entry_size(imd, e));
	}

	return 0;
}
