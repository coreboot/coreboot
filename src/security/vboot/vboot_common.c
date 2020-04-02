/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boot_device.h>
#include <cbmem.h>
#include <console/cbmem_console.h>
#include <fmap.h>
#include <reset.h>
#include <stddef.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/vbnv.h>
#include <vb2_api.h>

/* Check if it is okay to enable USB Device Controller (UDC). */
int vboot_can_enable_udc(void)
{
	/* Allow UDC in all vboot modes. */
	if (!CONFIG(CHROMEOS) && CONFIG(VBOOT_ALWAYS_ALLOW_UDC))
		return 1;

	/* Always disable if not in developer mode */
	if (!vboot_developer_mode_enabled())
		return 0;
	/* Enable if GBB flag is set */
	if (vboot_is_gbb_flag_set(VB2_GBB_FLAG_ENABLE_UDC))
		return 1;
	/* Enable if VBNV flag is set */
	if (vbnv_udc_enable_flag())
		return 1;
	/* Otherwise disable */
	return 0;
}

/* ============================ VBOOT REBOOT ============================== */
void __weak vboot_platform_prepare_reboot(void)
{
}

void vboot_reboot(void)
{
	if (CONFIG(CONSOLE_CBMEM_DUMP_TO_UART))
		cbmem_dump_console();
	vboot_platform_prepare_reboot();
	board_reset();
}
