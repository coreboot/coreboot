/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <cbfs.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <bootmode.h>

#include "vbt.h"

#define VBT_SIGNATURE 0x54425624

enum cb_err locate_vbt(struct region_device *rdev)
{
	uint32_t vbtsig = 0;
	struct cbfsf file_desc;

	if (cbfs_boot_locate(&file_desc, "vbt.bin", NULL) < 0) {
		printk(BIOS_ERR, "Could not locate a VBT file in in CBFS\n");
		return CB_ERR;
	}

	cbfs_file_data(rdev, &file_desc);
	rdev_readat(rdev, &vbtsig, 0, sizeof(uint32_t));

	if (vbtsig != VBT_SIGNATURE) {
		printk(BIOS_ERR, "Missing/invalid signature in VBT data file!\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

void *vbt_get(struct region_device *rdev)
{
	void *vbt_data;

	if (!IS_ENABLED(CONFIG_RUN_FSP_GOP))
		return NULL;

	/* Normal mode and S3 resume path PEIM GFX init is not needed.
	 * Passing NULL as VBT will not make PEIM GFX to execute. */
	if (acpi_is_wakeup_s3())
		return NULL;
	if (!display_init_required())
		return NULL;
	if (locate_vbt(rdev) != CB_ERR) {
		vbt_data = rdev_mmap_full(rdev);
		return vbt_data;
	} else {
		return NULL;
	}
}
