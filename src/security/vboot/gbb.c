/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <gbb_header.h>
#include <security/vboot/gbb.h>
#include <string.h>

#define GBB_FMAP_REGION_NAME	"GBB"

/* Copy of GBB header read from boot media. */
static GoogleBinaryBlockHeader gbb_header;

/*
 * Read "GBB" region from SPI flash to obtain GBB header and validate
 * signature.
 *
 * Return value:
 * Success = 0
 * Error = 1
 */
static int gbb_init(void)
{
	static bool init_done = false;
	struct region_device gbb_rdev;

	if (init_done != false)
		return 0;

	if (fmap_locate_area_as_rdev(GBB_FMAP_REGION_NAME, &gbb_rdev))
		return 1;

	if (rdev_readat(&gbb_rdev, &gbb_header, 0,
			sizeof(GoogleBinaryBlockHeader)) !=
	    sizeof(GoogleBinaryBlockHeader)) {
		printk(BIOS_ERR, "%s: Failure to read GBB header!\n", __func__);
		return 1;
	}

	if (memcmp(gbb_header.signature, GBB_SIGNATURE, GBB_SIGNATURE_SIZE)) {
		printk(BIOS_ERR, "%s: Signature check failed!\n", __func__);
		return 1;
	}

	init_done = true;
	return 0;
}

uint32_t gbb_get_flags(void)
{
	if (gbb_init()) {
		printk(BIOS_ERR,
		       "%s: Failure to initialize GBB. Returning flags as 0!\n",
		       __func__);
		return 0;
	}
	return gbb_header.flags;
}

bool gbb_is_flag_set(uint32_t flag)
{
	return !!(gbb_get_flags() & flag);
}
