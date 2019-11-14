/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#define NEED_VB20_INTERNALS  /* Peeking into vb2_shared_data */

#include <assert.h>
#include <bootmode.h>
#include <bootstate.h>
#include <cbmem.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>

static int vboot_get_recovery_reason_shared_data(void)
{
	struct vb2_shared_data *sd = vb2_get_sd(vboot_get_context());
	assert(sd);
	return sd->recovery_reason;
}

void vboot_save_recovery_reason_vbnv(void)
{
	if (!CONFIG(VBOOT_SAVE_RECOVERY_REASON_ON_REBOOT))
		return;

	int reason = vboot_get_recovery_reason_shared_data();
	if (!reason)
		return;

	set_recovery_mode_into_vbnv(reason);
}

static void vboot_clear_recovery_reason_vbnv(void *unused)
{
	if (!CONFIG(VBOOT_SAVE_RECOVERY_REASON_ON_REBOOT))
		return;

	set_recovery_mode_into_vbnv(0);
}

/*
 * Recovery reason stored in VBNV needs to be cleared before the state of VBNV
 * is backed-up anywhere or jumping to the payload (whichever occurs
 * first). Currently, vbnv_cmos.c backs up VBNV on POST_DEVICE. Thus, we need to
 * make sure that the stored recovery reason is cleared off before that
 * happens.
 * IMPORTANT: Any reboot occurring after BS_DEV_INIT state will cause loss of
 * recovery reason on reboot. Until now, we have seen reboots occurring on x86
 * only in FSP stages which run before BS_DEV_INIT.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT,
		      vboot_clear_recovery_reason_vbnv, NULL);

/*
 * vb2_check_recovery_request looks up different components to identify if there
 * is a recovery request and returns appropriate reason code:
 * 1. Checks if recovery mode is initiated by EC. If yes, returns
 * VB2_RECOVERY_RO_MANUAL.
 * 2. Checks if recovery request is present in VBNV and returns the code read
 * from it.
 * 3. Checks if vboot verification is done. If yes, return the reason code from
 * shared data.
 * 4. If nothing applies, return 0 indicating no recovery request.
 */
int vboot_check_recovery_request(void)
{
	int reason = 0;

	/* EC-initiated recovery. */
	if (get_recovery_mode_switch())
		return VB2_RECOVERY_RO_MANUAL;

	/* Recovery request in VBNV. */
	if ((reason = get_recovery_mode_from_vbnv()) != 0)
		return reason;

	/* Identify if vboot verification is already complete. */
	if (vboot_logic_executed())
		return vboot_get_recovery_reason_shared_data();

	return 0;
}

int vboot_recovery_mode_enabled(void)
{
	return !!vboot_check_recovery_request();
}

int __weak clear_recovery_mode_switch(void)
{
	// Weak implementation. Nothing to do.
	return 0;
}

void __weak log_recovery_mode_switch(void)
{
	// Weak implementation. Nothing to do.
}

int __weak get_recovery_mode_retrain_switch(void)
{
	return 0;
}

int vboot_recovery_mode_memory_retrain(void)
{
	return get_recovery_mode_retrain_switch();
}

int vboot_developer_mode_enabled(void)
{
	return vboot_logic_executed() &&
		vboot_get_context()->flags & VB2_CONTEXT_DEVELOPER_MODE;
}

#if CONFIG(VBOOT_NO_BOARD_SUPPORT)
/**
 * TODO: Create flash protection interface which implements get_write_protect_state.
 * get_recovery_mode_switch should be implemented as default function.
 */
int __weak get_write_protect_state(void)
{
	return 0;
}

int __weak get_recovery_mode_switch(void)
{
	return 0;
}

#endif
