/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <elog.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <vb2_api.h>

static void elog_add_boot_reason(void)
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

	/* Diagnostic boot if requested */
	if (vboot_get_context()->boot_mode == VB2_BOOT_MODE_DIAGNOSTICS) {
		elog_add_event_byte(ELOG_TYPE_CROS_DIAGNOSTICS,
				    ELOG_CROS_LAUNCH_DIAGNOSTICS);
		printk(BIOS_DEBUG, "%s: Logged diagnostic boot\n", __func__);
	}
}

static void elog_add_vboot_info(void)
{
	/* Skip logging boot info in ACPI resume path */
	if (acpi_is_wakeup_s3())
		return;

	struct vb2_context *ctx = vboot_get_context();
	union vb2_fw_boot_info data = vb2api_get_fw_boot_info(ctx);
	uint8_t width = offsetof(union vb2_fw_boot_info, recovery_reason);

	if (vboot_recovery_mode_enabled())
		width = sizeof(union vb2_fw_boot_info);

	elog_add_event_raw(ELOG_TYPE_FW_VBOOT_INFO, &data, width);
}

static void elog_add_boot_records(void *unused)
{
	/* Log boot reason into the eventlog */
	elog_add_boot_reason();
	/* Log fw vboot info into the eventlog */
	elog_add_vboot_info();
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, elog_add_boot_records, NULL);
