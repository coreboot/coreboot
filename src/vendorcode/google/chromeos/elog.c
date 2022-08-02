/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <elog.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <vb2_api.h>

static void elog_add_vboot_info(void *unused)
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

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, elog_add_vboot_info, NULL);
