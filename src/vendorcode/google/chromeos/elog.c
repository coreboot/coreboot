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

#include <cbmem.h>
#include <console/console.h>
#include <elog.h>
#include <vendorcode/google/chromeos/chromeos.h>
#if CONFIG_VBOOT_VERIFY_FIRMWARE
#include "vboot_handoff.h"
#include <vboot_struct.h>
#endif

void elog_add_boot_reason(void)
{
	if (developer_mode_enabled()) {
		elog_add_event(ELOG_TYPE_CROS_DEVELOPER_MODE);
		printk(BIOS_DEBUG, "%s: Logged dev mode boot\n", __func__);
	} else if (recovery_mode_enabled()) {
		u8 reason = 0;
#if CONFIG_VBOOT_VERIFY_FIRMWARE
		struct vboot_handoff *vbho = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

		reason = get_recovery_mode_from_vbnv();
		if (vbho && !reason) {
			VbSharedDataHeader *sd = (VbSharedDataHeader *)
				vbho->shared_data;
			reason = sd->recovery_reason;
		}
#endif

		elog_add_event_byte(ELOG_TYPE_CROS_RECOVERY_MODE,
			reason ? reason : ELOG_CROS_RECOVERY_MODE_BUTTON);
		printk(BIOS_DEBUG, "%s: Logged recovery mode boot, "
				"reason: 0x%02x\n", __func__, reason);
	} else {
		printk(BIOS_DEBUG, "%s: Normal mode boot, nothing "
					"interesting to log\n", __func__);
	}
}
