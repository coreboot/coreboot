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
