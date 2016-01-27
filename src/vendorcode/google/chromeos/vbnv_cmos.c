/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <bootstate.h>
#include <console/console.h>
#include <types.h>
#include <pc80/mc146818rtc.h>
#include "vbnv.h"
#include "vbnv_layout.h"

void read_vbnv_cmos(uint8_t *vbnv_copy)
{
	int i;

	for (i = 0; i < VBNV_BLOCK_SIZE; i++)
		vbnv_copy[i] = cmos_read(CONFIG_VBNV_OFFSET + 14 + i);

	if (IS_ENABLED(CONFIG_CHROMEOS_VBNV_CMOS_BACKUP_TO_FLASH)) {
		if (verify_vbnv(vbnv_copy))
			return;

		printk(BIOS_INFO, "VBNV: CMOS invalid, restoring from flash\n");
		read_vbnv_flash(vbnv_copy);

		if (verify_vbnv(vbnv_copy)) {
			save_vbnv_cmos(vbnv_copy);
			printk(BIOS_INFO, "VBNV: Flash backup restored\n");
		} else {
			printk(BIOS_INFO, "VBNV: Restore from flash failed\n");
		}
	}
}

void save_vbnv_cmos(const uint8_t *vbnv_copy)
{
	int i;

	for (i = 0; i < VBNV_BLOCK_SIZE; i++)
		cmos_write(vbnv_copy[i], CONFIG_VBNV_OFFSET + 14 + i);
}

#if IS_ENABLED(CONFIG_CHROMEOS_VBNV_CMOS_BACKUP_TO_FLASH)
static void backup_vbnv_cmos(void *unused)
{
	uint8_t vbnv_cmos[VBNV_BLOCK_SIZE];

	/* Read current VBNV from CMOS. */
	read_vbnv_cmos(vbnv_cmos);

	/* Save to flash, will only be saved if different. */
	save_vbnv_flash(vbnv_cmos);
}
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, backup_vbnv_cmos, NULL);
#endif
