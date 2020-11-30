/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <fsp/util.h>
#include <timestamp.h>
#include <mode_switch.h>

static void fsp_notify(enum fsp_notify_phase phase)
{
	uint32_t ret;
	fsp_notify_fn fspnotify;
	struct fsp_notify_params notify_params = { .phase = phase };

	if (!fsps_hdr.notify_phase_entry_offset)
		die("Notify_phase_entry_offset is zero!\n");

	fspnotify = (void *) (uintptr_t)(fsps_hdr.image_base +
			    fsps_hdr.notify_phase_entry_offset);
	fsp_before_debug_notify(fspnotify, &notify_params);

	if (phase == AFTER_PCI_ENUM) {
		timestamp_add_now(TS_FSP_BEFORE_ENUMERATE);
		post_code(POST_FSP_NOTIFY_BEFORE_ENUMERATE);
	} else if (phase == READY_TO_BOOT) {
		timestamp_add_now(TS_FSP_BEFORE_FINALIZE);
		post_code(POST_FSP_NOTIFY_BEFORE_FINALIZE);
	} else if (phase == END_OF_FIRMWARE) {
		timestamp_add_now(TS_FSP_BEFORE_END_OF_FIRMWARE);
		post_code(POST_FSP_NOTIFY_BEFORE_END_OF_FIRMWARE);
	}

	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		ret = protected_mode_call_1arg(fspnotify, (uintptr_t)&notify_params);
	else
		ret = fspnotify(&notify_params);

	if (phase == AFTER_PCI_ENUM) {
		timestamp_add_now(TS_FSP_AFTER_ENUMERATE);
		post_code(POST_FSP_NOTIFY_BEFORE_ENUMERATE);
	} else if (phase == READY_TO_BOOT) {
		timestamp_add_now(TS_FSP_AFTER_FINALIZE);
		post_code(POST_FSP_NOTIFY_BEFORE_FINALIZE);
	} else if (phase == END_OF_FIRMWARE) {
		timestamp_add_now(TS_FSP_AFTER_END_OF_FIRMWARE);
		post_code(POST_FSP_NOTIFY_AFTER_END_OF_FIRMWARE);
	}
	fsp_debug_after_notify(ret);

	/* Handle any errors returned by FspNotify */
	fsp_handle_reset(ret);
	if (ret != FSP_SUCCESS) {
		printk(BIOS_SPEW, "FspNotify returned 0x%08x\n", ret);
		die("FspNotify returned an error!\n");
	}

	/* Allow the platform to run something after FspNotify */
	platform_fsp_notify_status(phase);
}

static void fsp_notify_dummy(void *arg)
{
	enum fsp_notify_phase phase = (uint32_t)(uintptr_t)arg;

	display_mtrrs();

	fsp_notify(phase);
	if (phase == READY_TO_BOOT)
		fsp_notify(END_OF_FIRMWARE);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fsp_notify_dummy,
						(void *) AFTER_PCI_ENUM);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, fsp_notify_dummy,
						(void *) READY_TO_BOOT);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, fsp_notify_dummy,
						(void *) READY_TO_BOOT);

__weak void platform_fsp_notify_status(
	enum fsp_notify_phase phase)
{
}
