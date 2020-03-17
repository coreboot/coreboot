/*
 * This file is part of the coreboot project.
 *
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

#ifndef _IMD_H_
#define _IMD_H_

#include <stdint.h>
#include <stddef.h>

/*
 * imd is an in-memory database/directory/datastore (whatever d word you
 * desire). It grows downwards in memory from provided upper limit and
 * root size. Each entry has a size alignment which is also provided by
 * the caller.
 *
 *             +----------------------+ <- upper_limit
 *    |   +----|   root pointer       |
 *    |   |    +----------------------+
 *    |   |    |                      |--------+
 *    |   +--->|   root block         |-----+  |
 *    |        +----------------------+-----|--|--- root_size
 *    |        |                      |     |  |
 *    |        |                      |     |  |
 *    |        |   alloc N            |<----+  |
 *    |        +----------------------+        |
 *    |        |                      |        |
 *    |        |                      |        |
 *   \|/       |   alloc N + 1        |<-------+
 *    v        +----------------------+
 *
 * The root_size in imd_create_empty() encompasses the root pointer
 * and root block. The root_size value, therefore, dictates the number
 * of allocations maintained by the imd.
 */

/*
 * NOTE: This API has the following calling conventions: all functions
 * returning int supply 0 on success or < 0 on error.
 */

struct imd_entry;
struct imd;

/*
 * Initialize handle to use for working with an imd. Upper limit is the
 * exclusive address to start allocating down from. This function needs
 * to be called at least once before any other imd related functions
 * can be used.
 */
void imd_handle_init(struct imd *imd, void *upper_limit);

/*
 * Initialize a handle with a shallow recovery. This function doesn't
 * verify every entry, but it does set up the root pointer. Because of
 * this behavior it's not very safe. However, the current CBMEM constraints
 * demand having these semantics.
 */
void imd_handle_init_partial_recovery(struct imd *imd);

/*
 * Create an empty imd with a specified root_size and each entry is aligned to
 * the provided entry_align. As noted above the root size encompasses the
 * root pointer and root block leading to the number of imd entries being a
 * function of the root_size parameter.
 */
int imd_create_empty(struct imd *imd, size_t root_size, size_t entry_align);

/*
 * Create an empty imd with both large and small allocations. The small
 * allocations come from a fixed imd stored internally within the large
 * imd. The region allocated for tracking the smaller allocations is dependent
 * on the small root_size and the large entry alignment by calculating the
 * number of entries within the small imd and multiplying that by the small
 * entry alignment.
 */
int imd_create_tiered_empty(struct imd *imd,
				size_t lg_root_size, size_t lg_entry_align,
				size_t sm_root_size, size_t sm_entry_align);

/*
 * Recover a previously created imd.
 */
int imd_recover(struct imd *imd);

/* Limit imd to provided max_size. */
int imd_limit_size(struct imd *imd, size_t max_size);

/* Lock down imd from further modifications. */
int imd_lockdown(struct imd *imd);

/* Fill in base address and size of region used by imd. */
int imd_region_used(struct imd *imd, void **base, size_t *size);

/* Add an entry to the imd. If id already exists NULL is returned. */
const struct imd_entry *imd_entry_add(const struct imd *imd, uint32_t id,
					size_t size);

/* Locate an entry within the imd. NULL is returned when not found. */
const struct imd_entry *imd_entry_find(const struct imd *imd, uint32_t id);

/* Find an existing entry or add a new one. */
const struct imd_entry *imd_entry_find_or_add(const struct imd *imd,
						uint32_t id, size_t size);

/* Returns size of entry or 0 on failure. */
size_t imd_entry_size(const struct imd *imd, const struct imd_entry *entry);

/* Returns pointer to region described by entry or NULL on failure. */
void *imd_entry_at(const struct imd *imd, const struct imd_entry *entry);

/* Returns id for the imd entry. */
uint32_t imd_entry_id(const struct imd *imd, const struct imd_entry *entry);

/* Attempt to remove entry from imd. */
int imd_entry_remove(const struct imd *imd, const struct imd_entry *entry);

/* Print the entry information provided by lookup with the specified size. */
struct imd_lookup {
	uint32_t id;
	const char *name;
};

int imd_print_entries(const struct imd *imd, const struct imd_lookup *lookup,
			size_t size);

struct imd_cursor;
/* Initialize an imd_cursor object to walk the IMD entries. */
int imd_cursor_init(const struct imd *imd, struct imd_cursor *cursor);

/* Retrieve the next imd entry the cursor is referencing. Returns NULL when
 * no more entries exist. */
const struct imd_entry *imd_cursor_next(struct imd_cursor *cursor);

/*
 * The struct imd is a handle for working with an in-memory directory.
 *
 * NOTE: Do not directly touch any fields within this structure. An imd pointer
 * is meant to be opaque, but the fields are exposed for stack allocation.
 */
struct imdr {
	uintptr_t limit;
	void *r;
};
struct imd {
	struct imdr lg;
	struct imdr sm;
};

struct imd_cursor {
	size_t current_imdr;
	size_t current_entry;
	const struct imdr *imdr[2];
};

#endif /* _IMD_H_ */
