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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <cbfs.h>
#include <console/console.h>
#include <fsp/ramstage.h>
#include <fsp/util.h>
#include <lib.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Locate VBT and pass it to FSP GOP */
void load_vbt(uint8_t s3_resume, SILICON_INIT_UPD *params)
{
	const optionrom_vbt_t *vbt_data;
	uint32_t vbt_len;

	/* Check boot mode - for S3 resume path VBT loading is not needed */
	if (s3_resume) {
		vbt_data = NULL;
		printk(BIOS_DEBUG, "S3 resume do not pass VBT to GOP\n");
	} else {
		if (developer_mode_enabled() || recovery_mode_enabled()) {
			/* Get VBT data */
			vbt_data = fsp_get_vbt(&vbt_len);
			if (vbt_data != NULL)
				printk(BIOS_DEBUG, "Passing VBT to GOP\n");
			else
				printk(BIOS_DEBUG, "VBT not found!\n");
		} else
			vbt_data = NULL;
	}
	params->GraphicsConfigPtr = (u32)vbt_data;
}
