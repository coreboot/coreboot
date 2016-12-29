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
#include <vboot/vbnv.h>
#include <vboot/vbnv_layout.h>

static void clear_vbnv_battery_cutoff_flag(uint8_t *vbnv_copy)
{
	/*
	 * Currently battery cutoff is done in payload stage, which does not
	 * update backup VBNV. And doing battery cutoff will invalidate CMOS.
	 * This means for every reboot after cutoff, read_vbnv_cmos will reload
	 * backup VBNV and try to cutoff again, causing endless reboot loop.
	 * So we should always clear battery cutoff flag from loaded backup.
	 */
	if (vbnv_copy[MISC_FLAGS_OFFSET] & MISC_FLAGS_BATTERY_CUTOFF_MASK) {
		printk(BIOS_INFO, "VBNV: Remove battery cut-off request.\n");
		vbnv_copy[MISC_FLAGS_OFFSET] &= ~MISC_FLAGS_BATTERY_CUTOFF_MASK;
		regen_vbnv_crc(vbnv_copy);
	}
}

void read_vbnv_cmos(uint8_t *vbnv_copy)
{
	int i;

	for (i = 0; i < VBOOT_VBNV_BLOCK_SIZE; i++)
		vbnv_copy[i] = cmos_read(CONFIG_VBOOT_VBNV_OFFSET + 14 + i);

	if (IS_ENABLED(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH)) {
		if (verify_vbnv(vbnv_copy))
			return;

		printk(BIOS_INFO, "VBNV: CMOS invalid, restoring from flash\n");
		read_vbnv_flash(vbnv_copy);

		if (verify_vbnv(vbnv_copy)) {
			clear_vbnv_battery_cutoff_flag(vbnv_copy);
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

	for (i = 0; i < VBOOT_VBNV_BLOCK_SIZE; i++)
		cmos_write(vbnv_copy[i], CONFIG_VBOOT_VBNV_OFFSET + 14 + i);
}

void init_vbnv_cmos(int rtc_fail)
{
	uint8_t vbnv[VBOOT_VBNV_BLOCK_SIZE];

	if (rtc_fail)
		read_vbnv_cmos(vbnv);

	cmos_init(rtc_fail);

	if (rtc_fail)
		save_vbnv_cmos(vbnv);
}

#if IS_ENABLED(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH)
static void back_up_vbnv_cmos(void *unused)
{
	uint8_t vbnv_cmos[VBOOT_VBNV_BLOCK_SIZE];

	/* Read current VBNV from CMOS. */
	read_vbnv_cmos(vbnv_cmos);

	/* Save to flash, will only be saved if different. */
	save_vbnv_flash(vbnv_cmos);
}
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, back_up_vbnv_cmos, NULL);
#endif
