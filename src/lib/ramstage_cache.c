/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stddef.h>
#include <string.h>
#include <cbfs.h>
#include <console/console.h>
#include <ramstage_cache.h>
#include <romstage_handoff.h>

#if CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM

void cache_loaded_ramstage(struct romstage_handoff *handoff,
                           const struct cbmem_entry *ramstage,
                           void *entry_point)
{
	struct ramstage_cache *cache;
	uint32_t total_size;
	uint32_t ramstage_size;
	void *ramstage_base;
	long cache_size = 0;

	ramstage_size = cbmem_entry_size(ramstage);
	ramstage_base = cbmem_entry_start(ramstage);

	cache = ramstage_cache_location(&cache_size);

	if (cache == NULL) {
		printk(BIOS_DEBUG, "No ramstage cache location provided.\n");
		return;
	}

	total_size = sizeof(*cache) + ramstage_size;
	if (total_size > cache_size) {
		printk(BIOS_DEBUG, "cache size too small: 0x%08x > 0x%08lx\n",
		       total_size, cache_size);
		/* Nuke whatever may be there now just in case. */
		cache->magic = ~RAMSTAGE_CACHE_MAGIC;
		return;
	}

	cache->magic = RAMSTAGE_CACHE_MAGIC;
	cache->entry_point = (uint32_t)entry_point;
	cache->load_address = (uint32_t)ramstage_base;
	cache->size = ramstage_size;

	printk(BIOS_DEBUG, "Saving ramstage to %p.\n", cache);

	/* Copy over the program. */
	memcpy(&cache->program[0], ramstage_base, ramstage_size);

	if (handoff == NULL)
		return;

	handoff->ramstage_entry_point = (uint32_t)entry_point;
}

void *load_cached_ramstage(struct romstage_handoff *handoff,
                           const struct cbmem_entry *ramstage)
{
	struct ramstage_cache *cache;
	long size = 0;

	cache = ramstage_cache_location(&size);

	if (!ramstage_cache_is_valid(cache)) {
		printk(BIOS_DEBUG, "Invalid ramstage cache found.\n");
		ramstage_cache_invalid(cache);
		return NULL;
	}

	printk(BIOS_DEBUG, "Loading ramstage from %p.\n", cache);

	memcpy((void *)cache->load_address, &cache->program[0], cache->size);

	return (void *)cache->entry_point;
}

#else

/* Cache relocated ramstage in CBMEM. */

void __attribute__((weak))
cache_loaded_ramstage(struct romstage_handoff *handoff,
                      const struct cbmem_entry *ramstage, void *entry_point)
{
	uint32_t ramstage_size;
	const struct cbmem_entry *entry;

	if (handoff == NULL)
		return;

	ramstage_size = cbmem_entry_size(ramstage);
	/* cbmem_entry_add() does a find() before add(). */
	entry = cbmem_entry_add(CBMEM_ID_RAMSTAGE_CACHE, ramstage_size);

	if (entry == NULL)
		return;

	/* Keep track of the entry point in the handoff structure. */
	handoff->ramstage_entry_point = (uint32_t)entry_point;

	memcpy(cbmem_entry_start(entry), cbmem_entry_start(ramstage),
	       ramstage_size);
}

void * __attribute__((weak))
load_cached_ramstage(struct romstage_handoff *handoff,
                     const struct cbmem_entry *ramstage)
{
	const struct cbmem_entry *entry_cache;

	if (handoff == NULL)
		return NULL;

	entry_cache = cbmem_entry_find(CBMEM_ID_RAMSTAGE_CACHE);

	if (entry_cache == NULL)
		return NULL;

	/* Load the cached ramstage copy into the to-be-run region. */
	memcpy(cbmem_entry_start(ramstage), cbmem_entry_start(entry_cache),
	       cbmem_entry_size(ramstage));

	return (void *)handoff->ramstage_entry_point;
}

#endif
