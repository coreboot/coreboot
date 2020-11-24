/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <pc80/mc146818rtc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/symbols.h>
#include <soc/psp_transfer.h>
#include <2struct.h>

void verify_psp_transfer_buf(void)
{
	if (*(uint32_t *)_vboot2_work == VB2_SHARED_DATA_MAGIC) {
		cmos_write(0x00, CMOS_RECOVERY_BYTE);
		return;
	}

	/*
	 * If CMOS is valid and the system has already been rebooted once, but
	 * still returns here, instead of rebooting to verstage again, assume
	 * that the system is in a reboot loop and halt.
	 */
	if ((!vbnv_cmos_failed()) && cmos_read(CMOS_RECOVERY_BYTE) ==
			CMOS_RECOVERY_MAGIC_VAL)
		die("Error: Reboot into recovery was unsuccessful.  Halting.");

	printk(BIOS_ERR, "ERROR: VBOOT workbuf not valid.\n");
	printk(BIOS_DEBUG, "Signature: %#08x\n", *(uint32_t *)_vboot2_work);
	cmos_init(0);
	cmos_write(CMOS_RECOVERY_MAGIC_VAL, CMOS_RECOVERY_BYTE);
	warm_reset();
}

void show_psp_transfer_info(void)
{
	struct transfer_info_struct *info = (struct transfer_info_struct *)
			(void *)(uintptr_t)_transfer_buffer;

	if (info->magic_val == TRANSFER_MAGIC_VAL) {
		if ((info->psp_info & PSP_INFO_VALID) == 0) {
			printk(BIOS_INFO, "No PSP info found in transfer buffer.\n");
			return;
		}

		printk(BIOS_INFO, "PSP boot mode: %s\n",
				info->psp_info & PSP_INFO_PRODUCTION_MODE ?
				"Production" : "Development");
		printk(BIOS_INFO, "Silicon level: %s\n",
				info->psp_info & PSP_INFO_PRODUCTION_SILICON ?
				"Production" : "Pre-Production");
	}
}
