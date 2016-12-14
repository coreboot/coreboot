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
#include <boot_device.h>
#include <bootstate.h>
#include <console/console.h>
#include <cbmem.h>
#include <elog.h>
#include <fmap.h>
#include <ip_checksum.h>
#include <region_file.h>
#include <vboot/vboot_common.h>

#include "mrc_cache.h"
#include "nvm.h"

#define DEFAULT_MRC_CACHE	"RW_MRC_CACHE"
#define VARIABLE_MRC_CACHE	"RW_VAR_MRC_CACHE"
#define RECOVERY_MRC_CACHE	"RECOVERY_MRC_CACHE"
#define UNIFIED_MRC_CACHE	"UNIFIED_MRC_CACHE"

#define MRC_DATA_SIGNATURE       (('M'<<0)|('R'<<8)|('C'<<16)|('D'<<24))

struct mrc_metadata {
	uint32_t signature;
	uint32_t data_size;
	uint16_t data_checksum;
	uint16_t header_checksum;
	uint32_t version;
} __attribute__((packed));

enum result {
	UPDATE_FAILURE		= -1,
	UPDATE_SUCCESS		= 0,
	ALREADY_UPTODATE	= 1
};

#define NORMAL_FLAG (1 << 0)
#define RECOVERY_FLAG (1 << 1)

struct cache_region {
	const char *name;
	uint32_t cbmem_id;
	int type;
	int elog_slot;
	int flags;
};

static const struct cache_region recovery_training = {
	.name = RECOVERY_MRC_CACHE,
	.cbmem_id = CBMEM_ID_MRCDATA,
	.type = MRC_TRAINING_DATA,
	.elog_slot = ELOG_MEM_CACHE_UPDATE_SLOT_RECOVERY,
#if IS_ENABLED(CONFIG_HAS_RECOVERY_MRC_CACHE)
	.flags = RECOVERY_FLAG,
#else
	.flags = 0,
#endif
};

static const struct cache_region normal_training = {
	.name = DEFAULT_MRC_CACHE,
	.cbmem_id = CBMEM_ID_MRCDATA,
	.type = MRC_TRAINING_DATA,
	.elog_slot = ELOG_MEM_CACHE_UPDATE_SLOT_NORMAL,
	.flags = NORMAL_FLAG | RECOVERY_FLAG,
};

static const struct cache_region variable_data = {
	.name = VARIABLE_MRC_CACHE,
	.cbmem_id = CBMEM_ID_VAR_MRCDATA,
	.type = MRC_VARIABLE_DATA,
	.elog_slot = ELOG_MEM_CACHE_UPDATE_SLOT_VARIABLE,
	.flags = NORMAL_FLAG | RECOVERY_FLAG,
};

/* Order matters here for priority in matching. */
static const struct cache_region *cache_regions[] = {
	&recovery_training,
	&normal_training,
	&variable_data,
};

static int lookup_region_by_name(const char *name, struct region *r)
{
	/* This assumes memory mapped boot media just under 4GiB. */
	const uint32_t pointer_base_32bit = -CONFIG_ROM_SIZE;

	if (fmap_locate_area(name, r) == 0)
		return 0;

	/* CHROMEOS builds must get their MRC cache from FMAP. */
	if (IS_ENABLED(CONFIG_CHROMEOS)) {
		printk(BIOS_ERR, "MRC: Chrome OS lookup failure.\n");
		return -1;
	}

	if (!IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED))
		return -1;

	/* Base is in the form of a pointer. Make it an offset. */
	r->offset = CONFIG_MRC_SETTINGS_CACHE_BASE - pointer_base_32bit;
	r->size = CONFIG_MRC_SETTINGS_CACHE_SIZE;

	return 0;
}

static const struct cache_region *lookup_region_type(int type)
{
	int i;
	int flags;

	if (vboot_recovery_mode_enabled())
		flags = RECOVERY_FLAG;
	else
		flags = NORMAL_FLAG;

	for (i = 0; i < ARRAY_SIZE(cache_regions); i++) {
		if (cache_regions[i]->type != type)
			continue;
		if ((cache_regions[i]->flags & flags) == flags)
			return cache_regions[i];
	}

	return NULL;
}

int mrc_cache_stash_data(int type, uint32_t version, const void *data,
			size_t size)
{
	const struct cache_region *cr;
	size_t cbmem_size;
	struct mrc_metadata *md;

	cr = lookup_region_type(type);
	if (cr == NULL) {
		printk(BIOS_ERR, "MRC: failed to add to cbmem for type %d.\n",
			type);
		return -1;
	}

	cbmem_size = sizeof(*md) + size;

	md = cbmem_add(cr->cbmem_id, cbmem_size);

	if (md == NULL) {
		printk(BIOS_ERR, "MRC: failed to add '%s' to cbmem.\n",
			cr->name);
		return -1;
	}

	memset(md, 0, sizeof(*md));
	md->signature = MRC_DATA_SIGNATURE;
	md->data_size = size;
	md->version = version;
	md->data_checksum = compute_ip_checksum(data, size);
	md->header_checksum = compute_ip_checksum(md, sizeof(*md));
	memcpy(&md[1], data, size);

	return 0;
}

static const struct cache_region *lookup_region(struct region *r, int type)
{
	const struct cache_region *cr;

	cr = lookup_region_type(type);

	if (cr == NULL) {
		printk(BIOS_ERR, "MRC: failed to locate region type %d.\n",
			type);
		return NULL;
	}

	if (lookup_region_by_name(cr->name, r) < 0)
		return NULL;

	return cr;
}

static int mrc_header_valid(struct region_device *rdev, struct mrc_metadata *md)
{
	uint16_t checksum;
	uint16_t checksum_result;
	size_t size;

	if (rdev_readat(rdev, md, 0, sizeof(*md)) < 0) {
		printk(BIOS_ERR, "MRC: couldn't read metadata\n");
		return -1;
	}

	if (md->signature != MRC_DATA_SIGNATURE) {
		printk(BIOS_ERR, "MRC: invalid header signature\n");
		return -1;
	}

	/* Compute checksum over header with 0 as the value. */
	checksum = md->header_checksum;
	md->header_checksum = 0;
	checksum_result = compute_ip_checksum(md, sizeof(*md));

	if (checksum != checksum_result) {
		printk(BIOS_ERR, "MRC: header checksum mismatch: %x vs %x\n",
			checksum, checksum_result);
		return -1;
	}

	/* Put back original. */
	md->header_checksum = checksum;

	/* Re-size the region device according to the metadata as a region_file
	 * does block allocation. */
	size = sizeof(*md) + md->data_size;
	if (rdev_chain(rdev, rdev, 0, size) < 0) {
		printk(BIOS_ERR, "MRC: size exceeds rdev size: %zx vs %zx\n",
			size, region_device_sz(rdev));
		return -1;
	}

	return 0;
}

static int mrc_data_valid(const struct region_device *rdev,
				const struct mrc_metadata *md)
{
	void *data;
	uint16_t checksum;
	const size_t md_size = sizeof(*md);
	const size_t data_size = md->data_size;

	data = rdev_mmap(rdev, md_size, data_size);
	if (data == NULL) {
		printk(BIOS_ERR, "MRC: mmap failure on data verification.\n");
		return -1;
	}

	checksum = compute_ip_checksum(data, data_size);

	rdev_munmap(rdev, data);
	if (md->data_checksum != checksum) {
		printk(BIOS_ERR, "MRC: data checksum mismatch: %x vs %x\n",
			md->data_checksum, checksum);
		return -1;
	}

	return 0;
}

static int mrc_cache_latest(const char *name,
				const struct region_device *backing_rdev,
				struct mrc_metadata *md,
				struct region_file *cache_file,
				struct region_device *rdev,
				bool fail_bad_data)
{
	/* Init and obtain a handle to the file data. */
	if (region_file_init(cache_file, backing_rdev) < 0) {
		printk(BIOS_ERR, "MRC: region file invalid in '%s'\n", name);
		return -1;
	}

	/* Provide a 0 sized region_device from here on out so the caller
	 * has a valid yet unusable region_device. */
	rdev_chain(rdev, backing_rdev, 0, 0);

	/* No data to return. */
	if (region_file_data(cache_file, rdev) < 0) {
		printk(BIOS_ERR, "MRC: no data in '%s'\n", name);
		return fail_bad_data ? -1 : 0;
	}

	/* Validate header and resize region to reflect actual usage on the
	 * saved medium (including metadata and data). */
	if (mrc_header_valid(rdev, md) < 0) {
		printk(BIOS_ERR, "MRC: invalid header in '%s'\n", name);
		return fail_bad_data ? -1 : 0;
	}

	/* Validate Data */
	if (mrc_data_valid(rdev, md) < 0) {
		printk(BIOS_ERR, "MRC: invalid data in '%s'\n", name);
		return fail_bad_data ? -1 : 0;
	}

	return 0;
}

int mrc_cache_get_current(int type, uint32_t version,
				struct region_device *rdev)
{
	const struct cache_region *cr;
	struct region region;
	struct region_device read_rdev;
	struct region_file cache_file;
	struct mrc_metadata md;
	size_t data_size;
	const size_t md_size = sizeof(md);
	const bool fail_bad_data = true;

	cr = lookup_region(&region, type);

	if (cr == NULL)
		return -1;

	if (boot_device_ro_subregion(&region, &read_rdev) < 0)
		return -1;

	if (mrc_cache_latest(cr->name, &read_rdev, &md, &cache_file, rdev,
		fail_bad_data) < 0)
		return -1;

	if (version != md.version) {
		printk(BIOS_INFO, "MRC: version mismatch: %x vs %x\n",
			md.version, version);
		return -1;
	}

	/* Re-size rdev to only contain the data. i.e. remove metadata. */
	data_size = md.data_size;
	return rdev_chain(rdev, rdev, md_size, data_size);
}

static bool mrc_cache_needs_update(const struct region_device *rdev,
				const struct cbmem_entry *to_be_updated)
{
	void *mapping;
	size_t size = region_device_sz(rdev);
	bool need_update = false;

	if (cbmem_entry_size(to_be_updated) != size)
		return true;

	mapping = rdev_mmap_full(rdev);

	if (memcmp(cbmem_entry_start(to_be_updated), mapping, size))
		need_update = true;

	rdev_munmap(rdev, mapping);

	return need_update;
}

static void log_event_cache_update(uint8_t slot, enum result res)
{
	const int type = ELOG_TYPE_MEM_CACHE_UPDATE;
	struct elog_event_mem_cache_update event = {
		.slot = slot
	};

	/* Filter through interesting events only */
	switch (res) {
	case UPDATE_FAILURE:
		event.status = ELOG_MEM_CACHE_UPDATE_STATUS_FAIL;
		break;
	case UPDATE_SUCCESS:
		event.status = ELOG_MEM_CACHE_UPDATE_STATUS_SUCCESS;
		break;
	default:
		return;
	}

	if (elog_add_event_raw(type, &event, sizeof(event)) < 0)
		printk(BIOS_ERR, "Failed to log mem cache update event.\n");
}

/* During ramstage this code purposefully uses incoherent transactions between
 * read and write. The read assumes a memory-mapped boot device that can be used
 * to quickly locate and compare the up-to-date data. However, when an update
 * is required it uses the writeable region access to perform the update. */
static void update_mrc_cache_by_type(int type)
{
	const struct cache_region *cr;
	struct region region;
	struct region_device read_rdev;
	struct region_device write_rdev;
	struct region_file cache_file;
	struct mrc_metadata md;
	const struct cbmem_entry *to_be_updated;
	struct incoherent_rdev backing_irdev;
	const struct region_device *backing_rdev;
	struct region_device latest_rdev;
	const bool fail_bad_data = false;

	cr = lookup_region(&region, type);

	if (cr == NULL)
		return;

	to_be_updated = cbmem_entry_find(cr->cbmem_id);
	if (to_be_updated == NULL) {
		printk(BIOS_ERR, "MRC: No data in cbmem for '%s'.\n",
			cr->name);
		return;
	}

	printk(BIOS_DEBUG, "MRC: Checking cached data update for '%s'.\n",
		cr->name);

	if (boot_device_ro_subregion(&region, &read_rdev) < 0)
		return;

	if (boot_device_rw_subregion(&region, &write_rdev) < 0)
		return;

	backing_rdev = incoherent_rdev_init(&backing_irdev, &region, &read_rdev,
						&write_rdev);

	if (backing_rdev == NULL)
		return;

	if (mrc_cache_latest(cr->name, backing_rdev, &md, &cache_file,
		&latest_rdev, fail_bad_data) < 0)
		return;

	if (!mrc_cache_needs_update(&latest_rdev, to_be_updated)) {
		log_event_cache_update(cr->elog_slot, ALREADY_UPTODATE);
		return;
	}

	printk(BIOS_DEBUG, "MRC: cache data '%s' needs update.\n", cr->name);

	if (region_file_update_data(&cache_file,
		cbmem_entry_start(to_be_updated),
		cbmem_entry_size(to_be_updated)) < 0)
		log_event_cache_update(cr->elog_slot, UPDATE_FAILURE);
	else
		log_event_cache_update(cr->elog_slot, UPDATE_SUCCESS);
}

/* Protect mrc region with a Protected Range Register */
static int protect_mrc_cache(const char *name)
{
	struct region region;

	if (!IS_ENABLED(CONFIG_MRC_SETTINGS_PROTECT))
		return 0;

	if (lookup_region_by_name(name, &region) < 0) {
		printk(BIOS_ERR, "MRC: Could not find region '%s'\n", name);
		return -1;
	}

	if (nvm_is_write_protected() <= 0) {
		printk(BIOS_INFO, "MRC: NOT enabling PRR for '%s'.\n", name);
		return 0;
	}

	if (nvm_protect(&region) < 0) {
		printk(BIOS_ERR, "MRC: ERROR setting PRR for '%s'.\n", name);
		return -1;
	}

	printk(BIOS_INFO, "MRC: Enabled Protected Range on '%s'.\n", name);
	return 0;
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

static void invalidate_normal_cache(void)
{
	struct region_file cache_file;
	struct region_device rdev;
	const char *name = DEFAULT_MRC_CACHE;
	const uint32_t invalid = ~MRC_DATA_SIGNATURE;

	/* Invalidate only on recovery mode with retraining enabled. */
	if (!vboot_recovery_mode_enabled())
		return;
	if (!vboot_recovery_mode_memory_retrain())
		return;

	if (fmap_locate_area_as_rdev_rw(name, &rdev) < 0) {
		printk(BIOS_ERR, "MRC: Couldn't find '%s' region. Invalidation failed\n",
			name);
		return;
	}

	if (region_file_init(&cache_file, &rdev) < 0) {
		printk(BIOS_ERR, "MRC: region file invalid for '%s'. Invalidation failed\n",
			name);
		return;
	}

	/* Push an update that consists of 4 bytes that is smaller than the
	 * MRC metadata as well as an invalid signature. */
	if (region_file_update_data(&cache_file, &invalid, sizeof(invalid)) < 0)
		printk(BIOS_ERR, "MRC: invalidation failed for '%s'.\n", name);
}

static void update_mrc_cache(void *unused)
{
	update_mrc_cache_by_type(MRC_TRAINING_DATA);

	if (IS_ENABLED(CONFIG_MRC_SETTINGS_VARIABLE_DATA))
		update_mrc_cache_by_type(MRC_VARIABLE_DATA);

	if (IS_ENABLED(CONFIG_MRC_CLEAR_NORMAL_CACHE_ON_RECOVERY_RETRAIN))
		invalidate_normal_cache();

	protect_mrc_region();
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, update_mrc_cache, NULL);
