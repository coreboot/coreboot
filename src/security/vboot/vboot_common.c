/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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

#include <boot/coreboot_tables.h>
#include <boot_device.h>
#include <cbmem.h>
#include <compiler.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <fmap.h>
#include <gbb_header.h>
#include <reset.h>
#include <rules.h>
#include <stddef.h>
#include <string.h>
#include <security/vboot/gbb.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/vbnv.h>

int vboot_named_region_device(const char *name, struct region_device *rdev)
{
	return fmap_locate_area_as_rdev(name, rdev);
}

int vboot_named_region_device_rw(const char *name, struct region_device *rdev)
{
	return fmap_locate_area_as_rdev_rw(name, rdev);
}

/* Check if it is okay to enable USB Device Controller (UDC). */
int vboot_can_enable_udc(void)
{
	/* Always disable if not in developer mode */
	if (!vboot_developer_mode_enabled())
		return 0;
	/* Enable if GBB flag is set */
	if (gbb_is_flag_set(GBB_FLAG_ENABLE_UDC))
		return 1;
	/* Enable if VBNV flag is set */
	if (vbnv_udc_enable_flag())
		return 1;
	/* Otherwise disable */
	return 0;
}

/* ========================== VBOOT HANDOFF APIs =========================== */
int vboot_get_handoff_info(void **addr, uint32_t *size)
{
	/*
	 * vboot_handoff is present only after cbmem comes online. If we are in
	 * pre-ram stage, then bail out early.
	 */
	if (ENV_BOOTBLOCK ||
	    (ENV_VERSTAGE && IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK)))
		return -1;

	struct vboot_handoff *vboot_handoff;
	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vboot_handoff == NULL)
		return -1;

	*addr = vboot_handoff;

	if (size)
		*size = sizeof(*vboot_handoff);
	return 0;
}

static int vboot_get_handoff_flag(uint32_t flag)
{
	struct vboot_handoff *vbho;

	/*
	 * If vboot_handoff cannot be found, return default value of flag as 0.
	 */
	if (vboot_get_handoff_info((void **)&vbho, NULL))
		return 0;

	return !!(vbho->init_params.out_flags & flag);
}

int vboot_handoff_skip_display_init(void)
{
	return !vboot_get_handoff_flag(VB_INIT_OUT_ENABLE_DISPLAY);
}

int vboot_handoff_check_developer_flag(void)
{
	return vboot_get_handoff_flag(VB_INIT_OUT_ENABLE_DEVELOPER);
}

int vboot_handoff_check_recovery_flag(void)
{
	return vboot_get_handoff_flag(VB_INIT_OUT_ENABLE_RECOVERY);
}

int vboot_handoff_get_recovery_reason(void)
{
	struct vboot_handoff *vbho;
	VbSharedDataHeader *sd;

	if (vboot_get_handoff_info((void **)&vbho, NULL))
		return 0;

	sd = (VbSharedDataHeader *)vbho->shared_data;

	return sd->recovery_reason;
}

/* ============================ VBOOT REBOOT ============================== */
void __weak vboot_platform_prepare_reboot(void)
{
}

void vboot_reboot(void)
{
	if (IS_ENABLED(CONFIG_CONSOLE_CBMEM_DUMP_TO_UART))
		cbmem_dump_console();
	vboot_platform_prepare_reboot();
	hard_reset();
	die("failed to reboot");
}
