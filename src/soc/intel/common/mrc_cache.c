/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <string.h>
#include <bootstate.h>
#include <console/console.h>
#include <cbmem.h>
#include <elog.h>
#include <fmap.h>
#include <ip_checksum.h>
#include <vboot/vboot_common.h>

#include "mrc_cache.h"
#include "nvm.h"

#define MRC_DATA_ALIGN           0x1000
#define MRC_DATA_SIGNATURE       (('M'<<0)|('R'<<8)|('C'<<16)|('D'<<24))

/* The mrc_data_region describes the larger non-volatile area to store
 * mrc_saved_data objects.*/
struct mrc_data_region {
	void *base;
	uint32_t size;
};

/* common code */
static int mrc_cache_get_region(const char *name,
				struct mrc_data_region *region)
{
	bool located_by_fmap = true;
	struct region_device rdev;

	region->base = NULL;
	region->size = 0;

	if (fmap_locate_area_as_rdev(name, &rdev))
		located_by_fmap =  false;

	/* CHROMEOS builds must get their MRC cache from FMAP. */
	if (IS_ENABLED(CONFIG_CHROMEOS) && !located_by_fmap)
		return -1;

	if (located_by_fmap) {
		region->size = region_device_sz(&rdev);
		region->base = rdev_mmap_full(&rdev);

		if (region->base == NULL)
			return -1;
	} else {
		region->base = (void *)CONFIG_MRC_SETTINGS_CACHE_BASE;
		region->size = CONFIG_MRC_SETTINGS_CACHE_SIZE;
	}

	return 0;
}

/* Protect mrc region with a Protected Range Register */
static int __protect_mrc_cache(const struct mrc_data_region *region,
			       const char *name)
{
	if (!IS_ENABLED(CONFIG_MRC_SETTINGS_PROTECT))
		return 0;

	if (nvm_is_write_protected() <= 0) {
		printk(BIOS_INFO, "MRC: NOT enabling PRR for %s.\n", name);
		return 1;
	}

	if (nvm_protect(region->base, region->size) < 0) {
		printk(BIOS_ERR, "MRC: ERROR setting PRR for %s.\n", name);
		return -1;
	}

	printk(BIOS_INFO, "MRC: Enabled Protected Range on %s.\n", name);
	return 0;
}

static int protect_mrc_cache(const char *name)
{
	struct mrc_data_region region;
	if (mrc_cache_get_region(name, &region) < 0) {
		printk(BIOS_ERR, "MRC: Could not find region %s\n", name);
		return -1;
	}

	return __protect_mrc_cache(&region, name);
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
                                   const struct mrc_saved_data **cache,
                                   uint32_t version)
{
	const struct mrc_saved_data *msd;
	const struct mrc_saved_data *verified_cache;
	int slot = 0;

	msd = region->base;

	verified_cache = NULL;

	while (mrc_cache_in_region(region, msd) &&
	       mrc_cache_valid(region, msd)) {
		verified_cache = msd;
		msd = next_cache_block(msd);
		slot++;
	}

	/*
	 * Update pointer to the most recently saved MRC data before returning
	 * any error. This ensures that the caller can use next available slot
	 * if required.
	 */
	*cache = verified_cache;

	if (verified_cache == NULL)
		return -1;

	if (verified_cache->version != version) {
		printk(BIOS_DEBUG, "MRC: cache version mismatch: %x vs %x\n",
			verified_cache->version, version);
		return -1;
	}

	printk(BIOS_DEBUG, "MRC: cache slot %d @ %p\n", slot-1, verified_cache);

	return 0;
}

int mrc_cache_get_current_from_region(const struct mrc_saved_data **cache,
				      uint32_t version,
				      const char *region_name)
{
	struct mrc_data_region region;

	if (!region_name) {
		printk(BIOS_ERR, "MRC: Requires memory retraining.\n");
		return -1;
	}

	printk(BIOS_ERR, "MRC: Using data from %s\n", region_name);

	if (mrc_cache_get_region(region_name, &region) < 0) {
		printk(BIOS_ERR, "MRC: Region %s not found. "
		       "Requires memory retraining.\n", region_name);
		return -1;
	}

	if (__mrc_cache_get_current(&region, cache, version) < 0) {
		printk(BIOS_ERR, "MRC: Valid slot not found in %s."
		       "Requires memory retraining.\n", region_name);
		return -1;
	}

	return 0;
}

int mrc_cache_get_current_with_version(const struct mrc_saved_data **cache,
				       uint32_t version)
{
	return mrc_cache_get_current_from_region(cache, version,
						 DEFAULT_MRC_CACHE);
}

int mrc_cache_get_current(const struct mrc_saved_data **cache)
{
	return mrc_cache_get_current_with_version(cache, 0);
}

/* Fill in mrc_saved_data structure with payload. */
static void mrc_cache_fill(struct mrc_saved_data *cache, const void *data,
                           size_t size, uint32_t version)
{
	cache->signature = MRC_DATA_SIGNATURE;
	cache->size = size;
	cache->version = version;
	memcpy(&cache->data[0], data, size);
	cache->checksum = compute_ip_checksum((void *)&cache->data[0],
	                                      cache->size);
}

int mrc_cache_stash_data_with_version(const void *data, size_t size,
					uint32_t version)
{
	int cbmem_size;
	struct mrc_saved_data *cache;

	cbmem_size = sizeof(*cache) + ALIGN(size, 16);

	cache = cbmem_add(CBMEM_ID_MRCDATA, cbmem_size);

	if (cache == NULL) {
		printk(BIOS_ERR, "MRC: No space in cbmem for training data.\n");
		return -1;
	}

	/* Clear alignment padding bytes at end of data. */
	memset(&cache->data[size], 0, cbmem_size - size - sizeof(*cache));

	printk(BIOS_DEBUG, "MRC: Relocate data from %p to %p (%zu bytes)\n",
	       data, cache, size);

	mrc_cache_fill(cache, data, size, version);

	return 0;
}

int mrc_cache_stash_data(const void *data, size_t size)
{
	return mrc_cache_stash_data_with_version(data, size, 0);
}

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

	if (slot_begin < region_begin || slot_begin >= region_end)
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

static void log_event_cache_update(uint8_t slot, uint8_t status)
{
	const int type = ELOG_TYPE_MEM_CACHE_UPDATE;
	struct elog_event_mem_cache_update event = {
		.slot = slot,
		.status = status,
	};

	if (elog_add_event_raw(type, &event, sizeof(event)) < 0)
		printk(BIOS_ERR, "Failed to log mem cache update event.\n");
}

static void update_mrc_region(void)
{
	const struct mrc_saved_data *current_boot;
	const struct mrc_saved_data *current_saved;
	const struct mrc_saved_data *next_slot;
	struct mrc_data_region region;
	const char *region_name = DEFAULT_MRC_CACHE;
	uint8_t slot = ELOG_MEM_CACHE_UPDATE_SLOT_NORMAL;

	printk(BIOS_DEBUG, "MRC: Updating cache data.\n");

	if (vboot_recovery_mode_enabled() &&
	    IS_ENABLED(CONFIG_HAS_RECOVERY_MRC_CACHE)) {
		region_name = RECOVERY_MRC_CACHE;
		slot = ELOG_MEM_CACHE_UPDATE_SLOT_RECOVERY;
	}

	printk(BIOS_ERR, "MRC: Cache region selected - %s\n", region_name);

	if (mrc_cache_get_region(region_name, &region)) {
		printk(BIOS_ERR, "MRC: Could not obtain cache region.\n");
		return;
	}

	current_boot = cbmem_find(CBMEM_ID_MRCDATA);
	if (!current_boot) {
		printk(BIOS_ERR, "MRC: No cache in cbmem.\n");
		return;
	}

	if (!mrc_cache_valid(&region, current_boot)) {
		printk(BIOS_ERR, "MRC: Cache data in cbmem invalid.\n");
		return;
	}

	current_saved = NULL;

	if (!__mrc_cache_get_current(&region, &current_saved,
					current_boot->version)) {
		if (current_saved->size == current_boot->size &&
		    !memcmp(&current_saved->data[0], &current_boot->data[0],
		            current_saved->size)) {
			printk(BIOS_DEBUG, "MRC: Cache up to date.\n");
			return;
		}
	}

	next_slot = mrc_cache_next_slot(&region, current_saved);

	if (!mrc_slot_valid(&region, next_slot, current_boot)) {
		printk(BIOS_DEBUG, "MRC: Slot @ %p is invalid.\n", next_slot);
		if (!nvm_is_erased(region.base, region.size)) {
			if (nvm_erase(region.base, region.size) < 0) {
				printk(BIOS_DEBUG, "MRC: Failure erasing "
				       "region %s.\n", region_name);
				return;
			}
		}
		next_slot = region.base;
	}

	if (nvm_write((void *)next_slot, current_boot,
	               current_boot->size + sizeof(*current_boot))) {
		printk(BIOS_DEBUG, "MRC: Failure writing MRC cache to %s:%p\n",
		       region_name, next_slot);
		log_event_cache_update(slot, ELOG_MEM_CACHE_UPDATE_STATUS_FAIL);
	} else {
		log_event_cache_update(slot,
				       ELOG_MEM_CACHE_UPDATE_STATUS_SUCCESS);
	}
}

static void protect_mrc_region(void)
{
	/*
	 * Check if there is a single unified region that encompasses both
	 * RECOVERY_MRC_CACHE and DEFAULT_MRC_CACHE. In that case protect the
	 * entire region using a single PRR.
	 *
	 * If we are not able to protect the entire region, try protecting
	 * individual regions next.
	 */
	if (protect_mrc_cache(UNIFIED_MRC_CACHE) == 0)
		return;

	if (IS_ENABLED(CONFIG_HAS_RECOVERY_MRC_CACHE))
		protect_mrc_cache(RECOVERY_MRC_CACHE);

	protect_mrc_cache(DEFAULT_MRC_CACHE);
}

static void update_mrc_cache(void *unused)
{
	update_mrc_region();
	protect_mrc_region();
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, update_mrc_cache, NULL);
