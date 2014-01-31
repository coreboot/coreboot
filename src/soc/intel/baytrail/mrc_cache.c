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

#include <string.h>
#include <console/console.h>
#include <cbmem.h>
#include <ip_checksum.h>
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/fmap.h>
#endif
#include <baytrail/mrc_cache.h>

#define MRC_DATA_ALIGN           0x1000
#define MRC_DATA_SIGNATURE       (('M'<<0)|('R'<<8)|('C'<<16)|('D'<<24))

/* The mrc_data_region describes the larger non-volatile area to store
 * mrc_saved_data objects.*/
struct mrc_data_region {
	void *base;
	uint32_t size;
};

/* common code */
static int mrc_cache_get_region(struct mrc_data_region *region)
{
#if CONFIG_CHROMEOS
	int ret;
	ret = find_fmap_entry("RW_MRC_CACHE", &region->base);
	if (ret >= 0) {
		region->size = ret;
		return 0;
	}
#endif
	region->base = (void *)CONFIG_MRC_SETTINGS_CACHE_BASE;
	region->size = CONFIG_MRC_SETTINGS_CACHE_SIZE;
	return 0;
}

static int mrc_cache_in_region(const struct mrc_data_region *region,
                               const struct mrc_saved_data *cache)
{
	uintptr_t region_end;
	uintptr_t cache_end;

	if ((uintptr_t)cache < (uintptr_t)region->base)
		return 0;

	region_end = (uintptr_t)region->base;
	region_end += region->size;

	if ((uintptr_t)cache >= region_end)
		return 0;

	if ((sizeof(*cache) + (uintptr_t)cache) >= region_end)
		return 0;

	cache_end = (uintptr_t)cache;
	cache_end += cache->size + sizeof(*cache);

	if (cache_end > region_end)
		return 0;

	return 1;
}

static int mrc_cache_valid(const struct mrc_data_region *region,
                           const struct mrc_saved_data *cache)
{
	uint32_t checksum;

	if (cache->signature != MRC_DATA_SIGNATURE)
		return 0;

	if (cache->size > region->size)
		return 0;

	if (cache->reserved != 0)
		return 0;

	checksum = compute_ip_checksum((void *)&cache->data[0], cache->size);

	if (cache->checksum != checksum)
		return 0;

	return 1;
}

static const struct mrc_saved_data *
next_cache_block(const struct mrc_saved_data *cache)
{
	uintptr_t next = (uintptr_t)cache;

	next += ALIGN(cache->size + sizeof(*cache), MRC_DATA_ALIGN);

	return (const struct mrc_saved_data *)next;
}

/* Locate the most recently saved MRC data. */
static int __mrc_cache_get_current(const struct mrc_data_region *region,
                                   const struct mrc_saved_data **cache)
{
	const struct mrc_saved_data *msd;
	const struct mrc_saved_data *verified_cache;

	msd = region->base;

	verified_cache = NULL;

	while (mrc_cache_in_region(region, msd) &&
	       mrc_cache_valid(region, msd)) {
		verified_cache = msd;
		msd = next_cache_block(msd);
	}

	if (verified_cache == NULL)
		return -1;

	*cache = verified_cache;
	return 0;
}

int mrc_cache_get_current(const struct mrc_saved_data **cache)
{
	struct mrc_data_region region;

	if (mrc_cache_get_region(&region) < 0)
		return -1;

	return __mrc_cache_get_current(&region, cache);
}

#if defined(__PRE_RAM__)
/* romstage code */

/* Fill in mrc_saved_data structure with payload. */
static void mrc_cache_fill(struct mrc_saved_data *cache, void *data,
                           size_t size)
{
	cache->signature = MRC_DATA_SIGNATURE;
	cache->size = size;
	cache->reserved = 0;
	memcpy(&cache->data[0], data, size);
	cache->checksum = compute_ip_checksum((void *)&cache->data[0],
	                                      cache->size);
}

int mrc_cache_stash_data(void *data, size_t size)
{
	int cbmem_size;
	struct mrc_saved_data *cache;

	cbmem_size = sizeof(*cache) + ALIGN(size, 16);

	cache = cbmem_add(CBMEM_ID_MRCDATA, cbmem_size);

	if (cache == NULL) {
		printk(BIOS_ERR, "No space in cbmem for MRC data.\n");
		return -1;
	}

	/* Clear alignment padding bytes. */
	memset(&cache->data[size], 0, cbmem_size - size);

	printk(BIOS_DEBUG, "Relocate MRC DATA from %p to %p (%zu bytes)\n",
	       data, cache, size);

	mrc_cache_fill(cache, data, size);

	return 0;
}

#else
/* ramstage code */
#include <bootstate.h>
#include <baytrail/nvm.h>

static int mrc_slot_valid(const struct mrc_data_region *region,
                          const struct mrc_saved_data *slot,
                          const struct mrc_saved_data *to_save)
{
	uintptr_t region_begin;
	uintptr_t region_end;
	uintptr_t slot_end;
	uintptr_t slot_begin;
	uint32_t size;

	region_begin = (uintptr_t)region->base;
	region_end = region_begin + region->size;
	slot_begin = (uintptr_t)slot;
	size = to_save->size + sizeof(*to_save);
	slot_end = slot_begin + size;

	if (slot_begin < region_begin || slot_begin > region_end)
		return 0;

	if (size > region->size)
		return 0;

	if (slot_end > region_end || slot_end < region_begin)
		return 0;

	if (!nvm_is_erased(slot, size))
		return 0;

	return 1;
}

static const struct mrc_saved_data *
mrc_cache_next_slot(const struct mrc_data_region *region,
                    const struct mrc_saved_data *current_slot)
{
	const struct mrc_saved_data *next_slot;

	if (current_slot == NULL) {
		next_slot = region->base;
	} else {
		next_slot = next_cache_block(current_slot);
	}

	return next_slot;
}

static void update_mrc_cache(void *unused)
{
	const struct mrc_saved_data *current_boot;
	const struct mrc_saved_data *current_saved;
	const struct mrc_saved_data *next_slot;
	struct mrc_data_region region;

	printk(BIOS_DEBUG, "Updating MRC cache data.\n");

	current_boot = cbmem_find(CBMEM_ID_MRCDATA);
	if (!current_boot) {
		printk(BIOS_ERR, "No MRC cache in cbmem.\n");
		return;
	}

	if (mrc_cache_get_region(&region)) {
		printk(BIOS_ERR, "Could not obtain MRC cache region.\n");
		return;
	}

	if (!mrc_cache_valid(&region, current_boot)) {
		printk(BIOS_ERR, "MRC cache data in cbmem invalid.\n");
		return;
	}

	current_saved = NULL;

	if (!__mrc_cache_get_current(&region, &current_saved)) {
		if (current_saved->size == current_boot->size &&
		    !memcmp(&current_saved->data[0], &current_boot->data[0],
		            current_saved->size)) {
			printk(BIOS_DEBUG, "MRC cache up to date.\n");
			return;
		}
	}

	next_slot = mrc_cache_next_slot(&region, current_saved);

	if (!mrc_slot_valid(&region, next_slot, current_boot)) {
		if (nvm_erase(region.base, region.size) < 0) {
			printk(BIOS_DEBUG, "Could not erase MRC region.\n");
			return;
		}
		next_slot = region.base;
	}

	nvm_write((void *)next_slot, current_boot,
	          current_boot->size + sizeof(*current_boot));
}

BOOT_STATE_INIT_ENTRIES(mrc_cache_update) = {
	BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
	                      update_mrc_cache, NULL),
};

#endif /* defined(__PRE_RAM__) */
