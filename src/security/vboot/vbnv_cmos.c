/*
 * This file is part of the coreboot project.
 *
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
#include <security/vboot/vbnv.h>
#include <security/vboot/vbnv_layout.h>

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

/* Return non-zero if backup was used. */
static int restore_from_backup(uint8_t *vbnv_copy)
{
	if (!CONFIG(VBOOT_VBNV_CMOS_BACKUP_TO_FLASH))
		return 0;

	printk(BIOS_INFO, "VBNV: CMOS invalid, restoring from flash\n");
	read_vbnv_flash(vbnv_copy);

	if (verify_vbnv(vbnv_copy)) {
		clear_vbnv_battery_cutoff_flag(vbnv_copy);
		save_vbnv_cmos(vbnv_copy);
		printk(BIOS_INFO, "VBNV: Flash backup restored\n");
		return 1;
	}

	printk(BIOS_INFO, "VBNV: Restore from flash failed\n");

	return 0;
}

void read_vbnv_cmos(uint8_t *vbnv_copy)
{
	int i;

	for (i = 0; i < VBOOT_VBNV_BLOCK_SIZE; i++)
		vbnv_copy[i] = cmos_read(CONFIG_VBOOT_VBNV_OFFSET + 14 + i);

	/* Verify contents before attempting a restore from backup storage. */
	if (verify_vbnv(vbnv_copy))
		return;

	restore_from_backup(vbnv_copy);
}

void save_vbnv_cmos(const uint8_t *vbnv_copy)
{
	int i;

	for (i = 0; i < VBOOT_VBNV_BLOCK_SIZE; i++)
		cmos_write(vbnv_copy[i], CONFIG_VBOOT_VBNV_OFFSET + 14 + i);
}

void vbnv_init_cmos(uint8_t *vbnv_copy)
{
	/* If no CMOS failure just defer to the normal read path for checking
	   vbnv contents' integrity. */
	if (!vbnv_cmos_failed())
		return;

	/* In the case of CMOS failure force the backup. If backup wasn't used
	   force the vbnv CMOS to be reset. */
	if (!restore_from_backup(vbnv_copy)) {
		vbnv_reset(vbnv_copy);
		/* This parallels the vboot_reference implementation. */
		vbnv_copy[HEADER_OFFSET] = HEADER_SIGNATURE |
			HEADER_FIRMWARE_SETTINGS_RESET |
			HEADER_KERNEL_SETTINGS_RESET;
		regen_vbnv_crc(vbnv_copy);
		save_vbnv_cmos(vbnv_copy);
	}
}

#if CONFIG(VBOOT_VBNV_CMOS_BACKUP_TO_FLASH)
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
