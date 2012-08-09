/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

#include <stdint.h>
#include <string.h>
#include <console/console.h>
#include <cbfs.h>
#include <ip_checksum.h>
#include <device/device.h>
#include <cbmem.h>
#include "pei_data.h"
#include "sandybridge.h"
#include <spi.h>
#include <spi_flash.h>
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/fmap.h>
#endif

struct mrc_data_container *next_mrc_block(struct mrc_data_container *mrc_cache)
{
	/* MRC data blocks are aligned within the region */
	u32 mrc_size = sizeof(*mrc_cache) + mrc_cache->mrc_data_size;
	if (mrc_size & (MRC_DATA_ALIGN - 1UL)) {
		mrc_size &= ~(MRC_DATA_ALIGN - 1UL);
		mrc_size += MRC_DATA_ALIGN;
	}

	u8 *region_ptr = (u8*)mrc_cache;
	region_ptr += mrc_size;
	return (struct mrc_data_container *)region_ptr;
}

int is_mrc_cache(struct mrc_data_container *mrc_cache)
{
	return (!!mrc_cache) && (mrc_cache->mrc_signature == MRC_DATA_SIGNATURE);
}

/* Right now, the offsets for the MRC cache area are hard-coded in the
 * northbridge Kconfig if CONFIG_CHROMEOS is not set. In order to make
 * this more flexible, there are two of options:
 *  - Have each mainboard Kconfig supply a hard-coded offset
 *  - Use CBFS
 */
u32 get_mrc_cache_region(struct mrc_data_container **mrc_region_ptr)
{
	u32 region_size;
#if CONFIG_CHROMEOS
	region_size =  find_fmap_entry("RW_MRC_CACHE", (void **)mrc_region_ptr);
#else
	region_size = CONFIG_MRC_CACHE_SIZE;
	*mrc_region_ptr = (struct mrc_data_container *)
		(CONFIG_MRC_CACHE_BASE + CONFIG_MRC_CACHE_LOCATION);
#endif

	return region_size;
}

/* find the first empty field in the MRC cache area. If there's none, return
 * the first region. By testing for emptiness caller can detect if flash
 * needs to be erased.
 *
 * FIXME: that interface is crap
 */
struct mrc_data_container *find_next_mrc_cache(void)
{
	u32 entry_id = 0;
	struct mrc_data_container *mrc_cache = NULL;
	u32 region_size = get_mrc_cache_region(&mrc_cache);
	void *mrc_region = (void*)mrc_cache;

	if (mrc_cache == NULL) {
		printk(BIOS_ERR, "%s: could not find mrc cache area\n", __func__);
		return NULL;
	}

	/* Search for the first empty entry in the region */
	while (is_mrc_cache(mrc_cache)) {
		entry_id++;
		mrc_cache = next_mrc_block(mrc_cache);
		/* If we exceed the defined area, move to front */
		if ((void*)mrc_cache >= (void*)(mrc_region + region_size)) {
			mrc_cache = (struct mrc_data_container *)mrc_region;
			break;
		}
	}

	printk(BIOS_DEBUG, "picked entry %u from cache block when looking for empty block\n", entry_id);

	return mrc_cache;
}

struct mrc_data_container *find_current_mrc_cache(void)
{
	u32 entry_id = 0;
	struct mrc_data_container *mrc_next, *mrc_cache = NULL;
	u32 region_size = get_mrc_cache_region(&mrc_cache);
	void *mrc_region = (void*)mrc_cache;
	mrc_next = mrc_cache;

	if (mrc_cache == NULL) {
		printk(BIOS_ERR, "%s: could not find mrc cache area\n", __func__);
		return NULL;
	}

	if (mrc_cache->mrc_data_size == -1UL) {
		printk(BIOS_ERR, "%s: MRC cache not initialized?\n",  __func__);
		/* return non-initialized cache, so we can discern this
		 * from having no cache area at all
		 */
		return mrc_cache;
	}

	/* Search for the last filled entry in the region */
	while (is_mrc_cache(mrc_next)) {
		entry_id++;
		mrc_cache = mrc_next;
		mrc_next = next_mrc_block(mrc_cache);
		/* Stay in the mrc data region */
		if ((void*)mrc_next >= (void*)(mrc_region + region_size))
			break;
	}
	entry_id--;

	if (entry_id == -1) {
		printk(BIOS_ERR, "%s: No valid MRC cache found.\n", __func__);
		return NULL;
	}

	/* Verify checksum */
	if (mrc_cache->mrc_checksum !=
	    compute_ip_checksum(mrc_cache->mrc_data,
				mrc_cache->mrc_data_size)) {
		printk(BIOS_ERR, "%s: MRC cache checksum mismatch\n", __func__);
		return NULL;
	}

	printk(BIOS_DEBUG, "picked entry %u from cache block\n", entry_id);

	return mrc_cache;
}

/* SPI code needs malloc/free.
 * Also unknown if writing flash from XIP-flash code is a good idea
 */
#if !defined(__PRE_RAM__)
void update_mrc_cache(void)
{
	printk(BIOS_DEBUG, "Updating MRC cache data.\n");
	struct mrc_data_container *current = cbmem_find(CBMEM_ID_MRCDATA);
	if (!current) {
		printk(BIOS_ERR, "No MRC cache in cbmem. Can't update flash.\n");
		return;
	}
	if (current->mrc_data_size == -1) {
		printk(BIOS_ERR, "MRC cache data in cbmem invalid.\n");
		return;
	}

	/*
	 * we need to:
	 */
	//  0. compare MRC data to last mrc-cache block (exit if same)
	struct mrc_data_container *cache;
	if ((cache = find_current_mrc_cache()) == NULL) {
		printk(BIOS_DEBUG, "Failure looking for current last block.\n");
		return;
	}

	if ((cache->mrc_data_size == current->mrc_data_size) &&
			(memcmp(cache, current, cache->mrc_data_size) == 0)) {
		printk(BIOS_DEBUG,
			"MRC data in flash is up to date. No update.\n");
		return;
	}

	//  1. use spi_flash_probe() to find the flash, then
	spi_init();
	struct spi_flash *flash = spi_flash_probe(0, 0, 1000000, SPI_MODE_3);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return;
	}

	//  2. look up the first unused block
	cache = find_next_mrc_cache();
	if (!cache) {
		printk(BIOS_DEBUG, "Could not find MRC cache area\n");
		return;
	}

	//  3. if no such place exists, erase entire mrc-cache range & use block 0
	if (cache->mrc_data_size != -1) {
		printk(BIOS_DEBUG, "We need to erase the MRC cache region\n");
		flash->erase(flash, CONFIG_MRC_CACHE_LOCATION, CONFIG_MRC_CACHE_SIZE);
		/* we know we can start at the beginning again */
		get_mrc_cache_region(&cache);
	}
	//  4. write mrc data with flash->write()
	printk(BIOS_DEBUG, "Finally: write MRC cache update to flash\n");
	flash->write(flash, (u32)(((void*)cache)-CONFIG_MRC_CACHE_BASE), current->mrc_data_size + sizeof(*current), current);
}
#endif

