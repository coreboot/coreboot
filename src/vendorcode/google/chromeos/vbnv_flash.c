/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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
 *
 * TODO: Make this CAR-friendly in case we use it on x86 some day.
 */

#include <assert.h>
#include <console/console.h>
#include <spi_flash.h>
#include <string.h>
#include <vb2_api.h>
#include <vboot_nvstorage.h>
#include "chromeos.h"
#include "vbnv_layout.h"

#define BLOB_SIZE VB2_NVDATA_SIZE

/* FMAP descriptor of the NVRAM area */
static struct region_device nvram_region;

/* offset of the current nvdata in SPI flash */
static int blob_offset = -1;

/* Offset of the topmost nvdata blob in SPI flash */
static int top_offset;

/* cache of the current nvdata */
static uint8_t cache[BLOB_SIZE];

/* spi_flash struct used when saving data */
static struct spi_flash *spi_flash = NULL;

/*
 * This code assumes that flash is erased to 1-bits, and write operations can
 * only change 1-bits to 0-bits. So if the new contents only change 1-bits to
 * 0-bits, we can reuse the current blob.
 */
static inline uint8_t erase_value(void)
{
	return 0xff;
}

static inline int can_overwrite(uint8_t current, uint8_t new)
{
	return (current & new) == new;
}

static inline int is_initialized(void)
{
	return blob_offset >= 0;
}

static int init_vbnv(void)
{
	uint8_t buf[BLOB_SIZE];
	uint8_t empty_blob[BLOB_SIZE];
	int offset;
	int i;

	if (vboot_named_region_device("RW_NVRAM", &nvram_region) ||
	    region_device_sz(&nvram_region) < BLOB_SIZE) {
		printk(BIOS_ERR, "%s: failed to locate NVRAM\n", __func__);
		return 1;
	}

	/* Prepare an empty blob to compare against. */
	for (i = 0; i < BLOB_SIZE; i++)
		empty_blob[i] = erase_value();

	offset = 0;
	top_offset = region_device_sz(&nvram_region) - BLOB_SIZE;

	/*
	 * after the loop, offset is supposed to point the blob right before
	 * the first empty blob, the last blob in the nvram if there is no
	 * empty blob, or the base of the region if the nvram has never been
	 * used.
	 */
	for (i = 0; i <= top_offset; i += BLOB_SIZE) {
		if (rdev_readat(&nvram_region, buf, i, BLOB_SIZE) < 0) {
			printk(BIOS_ERR, "failed to read nvdata\n");
			return 1;
		}
		if (!memcmp(buf, empty_blob, BLOB_SIZE))
			break;
		offset = i;
	}

	/* reread the nvdata and write it to the cache */
	if (rdev_readat(&nvram_region, cache, offset, BLOB_SIZE) < 0) {
		printk(BIOS_ERR, "failed to read nvdata\n");
		return 1;
	}

	blob_offset = offset;

	return 0;
}

static void vbnv_is_erasable(void)
{
	/*
	 * We check whether the region is aligned or not in advance to ensure
	 * we can erase the region when it's all used up.
	 *
	 * The region offset & size are determined by fmap.dts yet the check can
	 * be confidently done only by the spi flash driver. We use the same
	 * check as the one used by spi_flash_cmd_erase, which happens to be
	 * common to all the spi flash parts we support.
	 *
	 * TODO: Check by calling can_erase implemented by each spi flash driver
	 */
	assert(!(region_device_offset(&nvram_region) % spi_flash->sector_size));
	assert(!(region_device_sz(&nvram_region) % spi_flash->sector_size));
}

static int vbnv_flash_probe(void)
{
	if (!spi_flash) {
		spi_flash = spi_flash_probe(CONFIG_BOOT_MEDIA_SPI_BUS, 0);
		if (!spi_flash) {
			printk(BIOS_ERR, "failed to probe spi flash\n");
			return 1;
		}
		/*
		 * Called here instead of init_vbnv to reduce impact on boot
		 * speed.
		 */
		vbnv_is_erasable();
	}
	return 0;
}

static int erase_nvram(void)
{
	if (vbnv_flash_probe())
		return 1;

	if (spi_flash->erase(spi_flash, region_device_offset(&nvram_region),
			     region_device_sz(&nvram_region))) {
		printk(BIOS_ERR, "failed to erase nvram\n");
		return 1;
	}

	printk(BIOS_INFO, "nvram is cleared\n");
	return 0;
}

void read_vbnv(uint8_t *vbnv_copy)
{
	if (!is_initialized())
		if (init_vbnv())
			return;  /* error */
	memcpy(vbnv_copy, cache, BLOB_SIZE);
}

void save_vbnv(const uint8_t *vbnv_copy)
{
	int new_offset;
	int i;

	if (!is_initialized())
		if (init_vbnv())
			return;  /* error */

	/* Bail out if there have been no changes. */
	if (!memcmp(vbnv_copy, cache, BLOB_SIZE))
		return;

	new_offset = blob_offset;

	/* See if we can overwrite the current blob with the new one */
	for (i = 0; i < BLOB_SIZE; i++) {
		if (!can_overwrite(cache[i], vbnv_copy[i])) {
			/* unable to overwrite. need to use the next blob */
			new_offset += BLOB_SIZE;
			if (new_offset > top_offset) {
				if (erase_nvram())
					return;  /* error */
				new_offset = 0;
			}
			break;
		}
	}

	if (!vbnv_flash_probe() &&
	    !spi_flash->write(spi_flash,
			      region_device_offset(&nvram_region) + new_offset,
			      BLOB_SIZE, vbnv_copy)) {
		/* write was successful. safely move pointer forward */
		blob_offset = new_offset;
		memcpy(cache, vbnv_copy, BLOB_SIZE);
	} else {
		printk(BIOS_ERR, "failed to save nvdata\n");
	}
}

int get_recovery_mode_from_vbnv(void)
{
	if (!is_initialized())
		init_vbnv();
	return cache[RECOVERY_OFFSET];
}
