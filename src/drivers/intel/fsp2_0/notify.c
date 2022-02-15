/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <fsp/util.h>
#include <mode_switch.h>
#include <timestamp.h>
#include <types.h>

struct fsp_notify_phase_data {
	enum fsp_notify_phase notify_phase;
	bool skip;
	uint8_t post_code_before;
	uint8_t post_code_after;
	enum timestamp_id timestamp_before;
	enum timestamp_id timestamp_after;
};

static const struct fsp_notify_phase_data notify_data[] = {
	{
		.notify_phase     = AFTER_PCI_ENUM,
		.skip             = !CONFIG(USE_FSP_NOTIFY_PHASE_POST_PCI_ENUM),
		.post_code_before = POST_FSP_NOTIFY_BEFORE_ENUMERATE,
		.post_code_after  = POST_FSP_NOTIFY_AFTER_ENUMERATE,
		.timestamp_before = TS_FSP_ENUMERATE_START,
		.timestamp_after  = TS_FSP_ENUMERATE_END,
	},
	{
		.notify_phase     = READY_TO_BOOT,
		.skip             = !CONFIG(USE_FSP_NOTIFY_PHASE_READY_TO_BOOT),
		.post_code_before = POST_FSP_NOTIFY_BEFORE_FINALIZE,
		.post_code_after  = POST_FSP_NOTIFY_AFTER_FINALIZE,
		.timestamp_before = TS_FSP_FINALIZE_START,
		.timestamp_after  = TS_FSP_FINALIZE_END,
	},
	{
		.notify_phase     = END_OF_FIRMWARE,
		.skip             = !CONFIG(USE_FSP_NOTIFY_PHASE_END_OF_FIRMWARE),
		.post_code_before = POST_FSP_NOTIFY_BEFORE_END_OF_FIRMWARE,
		.post_code_after  = POST_FSP_NOTIFY_AFTER_END_OF_FIRMWARE,
		.timestamp_before = TS_FSP_END_OF_FIRMWARE_START,
		.timestamp_after  = TS_FSP_END_OF_FIRMWARE_END,
	},
};

static const struct fsp_notify_phase_data *get_notify_phase_data(enum fsp_notify_phase phase)
{
	for (size_t i = 0; i < ARRAY_SIZE(notify_data); i++) {
		if (notify_data[i].notify_phase == phase)
			return &notify_data[i];
	}
	die("Unknown FSP notify phase %u\n", phase);
}

static void fsp_notify(enum fsp_notify_phase phase)
{
	const struct fsp_notify_phase_data *data = get_notify_phase_data(phase);
	struct fsp_notify_params notify_params = { .phase = phase };
	fsp_notify_fn fspnotify;
	uint32_t ret;

	if (data->skip) {
		printk(BIOS_INFO, "coreboot skipped calling FSP notify phase: %08x.\n", phase);
		return;
	}

	if (!fsps_hdr.notify_phase_entry_offset)
		die("Notify_phase_entry_offset is zero!\n");

	fspnotify = (void *)(uintptr_t)(fsps_hdr.image_base +
			    fsps_hdr.notify_phase_entry_offset);
	fsp_before_debug_notify(fspnotify, &notify_params);

	timestamp_add_now(data->timestamp_before);
	post_code(data->post_code_before);

	if (ENV_X86_64 && CONFIG(PLATFORM_USES_FSP2_X86_32))
		ret = protected_mode_call_1arg(fspnotify, (uintptr_t)&notify_params);
	else
		ret = fspnotify(&notify_params);

	timestamp_add_now(data->timestamp_after);
	post_code(data->post_code_after);

	fsp_debug_after_notify(ret);

	/* Handle any errors returned by FspNotify */
	fsp_handle_reset(ret);
	if (ret != FSP_SUCCESS)
		die("FspNotify returned with error 0x%08x!\n", ret);

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

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fsp_notify_dummy, (void *)AFTER_PCI_ENUM);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, fsp_notify_dummy, (void *)READY_TO_BOOT);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, fsp_notify_dummy, (void *)READY_TO_BOOT);

__weak void platform_fsp_notify_status(enum fsp_notify_phase phase)
{
}
