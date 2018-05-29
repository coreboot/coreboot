/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <bootmode.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/gop.h>
#include <fsp/ramstage.h>
#include <fsp/util.h>

/* Reading VBT table from flash */
const optionrom_vbt_t *fsp_get_vbt(uint32_t *vbt_len)
{
	size_t vbt_size;
	union {
		const optionrom_vbt_t *data;
		uint32_t *signature;
	} vbt;

	/* Locate the vbt file in cbfs */
	vbt.data = cbfs_boot_map_with_leak("vbt.bin", CBFS_TYPE_RAW, &vbt_size);
	if (!vbt.data) {
		printk(BIOS_INFO,
			"FSP_INFO: VBT data file (vbt.bin) not found in CBFS");
		return NULL;
	}

	/* Validate the vbt file */
	if (*vbt.signature != VBT_SIGNATURE) {
		printk(BIOS_WARNING,
			"FSP_WARNING: Invalid signature in VBT data file (vbt.bin)!\n");
		return NULL;
	}
	*vbt_len = vbt_size;
	printk(BIOS_DEBUG, "FSP_INFO: VBT found at %p, 0x%08x bytes\n",
		vbt.data, *vbt_len);

#if IS_ENABLED(CONFIG_DISPLAY_VBT)
	/* Display the vbt file contents */
	printk(BIOS_DEBUG, "VBT Data:\n");
	hexdump(vbt.data, *vbt_len);
	printk(BIOS_DEBUG, "\n");
#endif

	/* Return the pointer to the vbt file data */
	return vbt.data;
}

/* Locate VBT and pass it to FSP GOP */
void load_vbt(uint8_t s3_resume, SILICON_INIT_UPD *params)
{
	const optionrom_vbt_t *vbt_data = NULL;
	uint32_t vbt_len;

	/* Check boot mode - for S3 resume path VBT loading is not needed */
	if (s3_resume) {
		printk(BIOS_DEBUG, "S3 resume do not pass VBT to GOP\n");
	} else if (display_init_required()) {
		/* Get VBT data */
		vbt_data = fsp_get_vbt(&vbt_len);
		if (vbt_data != NULL)
			printk(BIOS_DEBUG, "Passing VBT to GOP\n");
		else
			printk(BIOS_DEBUG, "VBT not found!\n");
	} else {
		printk(BIOS_DEBUG, "Not passing VBT to GOP\n");
	}
	params->GraphicsConfigPtr = (u32)vbt_data;
}
