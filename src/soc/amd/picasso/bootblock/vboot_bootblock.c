/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
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
