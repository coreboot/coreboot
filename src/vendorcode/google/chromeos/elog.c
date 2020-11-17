/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <elog.h>
#include <security/vboot/vboot_common.h>

static void elog_add_boot_reason(void *unused)
{
	const int rec = vboot_recovery_mode_enabled();
	const int dev = vboot_developer_mode_enabled();

	if (rec) {
		const u8 reason = vboot_check_recovery_request();
		elog_add_event_byte(ELOG_TYPE_CROS_RECOVERY_MODE, reason);
		printk(BIOS_DEBUG, "%s: Logged recovery mode boot%s, reason: 0x%02x\n",
		       __func__, dev ? " (Dev-switch on)" : "", reason);
	}

	/* Skip logging developer mode in ACPI resume path */
	if (dev && !acpi_is_wakeup_s3()) {

		elog_add_event(ELOG_TYPE_CROS_DEVELOPER_MODE);
		printk(BIOS_DEBUG, "%s: Logged dev mode boot\n", __func__);
	}
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, elog_add_boot_reason, NULL);
