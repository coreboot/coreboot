/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <bootmode.h>
#include <console/console.h>
#include <stdint.h>

int platform_is_resuming(void)
{
	uint32_t bootmode = 0;
	if (svc_get_boot_mode(&bootmode)) {
		printk(BIOS_ERR, "Error getting boot mode. Assuming no resume.\n");
		return 0;
	}

	if (bootmode == PSP_BOOT_MODE_S3_RESUME || bootmode == PSP_BOOT_MODE_S0i3_RESUME)
		return 1;

	return 0;
}
