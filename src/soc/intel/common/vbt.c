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

static char vbt_data[8 * KiB];
static int vbt_data_used;

void *locate_vbt(void)
{
	uint32_t vbtsig = 0;

	if (vbt_data_used == 1)
		return (void *)vbt_data;

	const char *filename = mainboard_vbt_filename();

	size_t file_size = cbfs_boot_load_file(filename,
		vbt_data, sizeof(vbt_data), CBFS_TYPE_RAW);

	if (file_size == 0)
		return NULL;

	memcpy(&vbtsig, vbt_data, sizeof(vbtsig));
	if (vbtsig != VBT_SIGNATURE) {
		printk(BIOS_ERR, "Missing/invalid signature in VBT data file!\n");
		return NULL;
	}

	printk(BIOS_INFO, "Found a VBT of %zu bytes after decompression\n",
		file_size);
	vbt_data_used = 1;

	return (void *)vbt_data;
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
