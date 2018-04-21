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
#include <compiler.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <bootmode.h>
#include <bootstate.h>

#include "vbt.h"

#define VBT_SIGNATURE 0x54425624

__weak
const char *mainboard_vbt_filename(void)
{
	return "vbt.bin";
}

static struct region_device vbt_rdev;
static void *vbt_data;

void *locate_vbt(void)
{
	uint32_t vbtsig = 0;
	struct cbfsf file_desc;

	if (vbt_data != NULL)
		return vbt_data;

	const char *filename = mainboard_vbt_filename();

	if (cbfs_boot_locate(&file_desc, filename, NULL) < 0) {
		printk(BIOS_ERR, "Could not locate a VBT file in in CBFS\n");
		return NULL;
	}

	cbfs_file_data(&vbt_rdev, &file_desc);
	rdev_readat(&vbt_rdev, &vbtsig, 0, sizeof(uint32_t));

	if (vbtsig != VBT_SIGNATURE) {
		printk(BIOS_ERR, "Missing/invalid signature in VBT data file!\n");
		return NULL;
	}

	vbt_data = rdev_mmap_full(&vbt_rdev);
	return vbt_data;
}

void *vbt_get(void)
{
	if (!IS_ENABLED(CONFIG_RUN_FSP_GOP))
		return NULL;

	/* Normal mode and S3 resume path PEIM GFX init is not needed.
	 * Passing NULL as VBT will not make PEIM GFX to execute. */
	if (acpi_is_wakeup_s3())
		return NULL;
	if (!display_init_required())
		return NULL;
	return locate_vbt();
}

static void unmap_vbt(void *unused)
{
	if (vbt_data)
		rdev_munmap(&vbt_rdev, vbt_data);
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, unmap_vbt, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, unmap_vbt, NULL);
