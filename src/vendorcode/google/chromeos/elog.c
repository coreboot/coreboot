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
	/*
	 * Skip logging boot info if CSE sync scheduled at payload.
	 * The payload should log boot info after CSE sync.
	 */
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

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

#if CONFIG(POSTPONE_SPI_ACCESS)
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, elog_add_vboot_info, NULL);
#else
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, elog_add_vboot_info, NULL);
#endif
