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

#include <assert.h>
#include <bootmode.h>
#include <bootstate.h>
#include <cbmem.h>
#include <compiler.h>
#include <rules.h>
#include <string.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>

static int vb2_get_recovery_reason_shared_data(void)
{
	/* Shared data does not exist for Ramstage and Post-CAR stage. */
	if (ENV_RAMSTAGE || ENV_POSTCAR)
		return 0;

	struct vb2_shared_data *sd = vb2_get_shared_data();
	assert(sd);
	return sd->recovery_reason;
}

void vb2_save_recovery_reason_vbnv(void)
{
	if (!IS_ENABLED(CONFIG_VBOOT_SAVE_RECOVERY_REASON_ON_REBOOT))
		return;

	int reason =  vb2_get_recovery_reason_shared_data();
	if (!reason)
		return;

	set_recovery_mode_into_vbnv(reason);
}

static void vb2_clear_recovery_reason_vbnv(void *unused)
{
	if (!IS_ENABLED(CONFIG_VBOOT_SAVE_RECOVERY_REASON_ON_REBOOT))
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
 * recovery reason on reboot. Until now, we have seen reboots occuring on x86
 * only in FSP stages which run before BS_DEV_INIT.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT,
		      vb2_clear_recovery_reason_vbnv, NULL);

/*
 * Returns 1 if vboot is being used and currently in a stage which might have
 * already executed vboot verification.
 */
static int vboot_possibly_executed(void)
{
	if (IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK)) {
		if (ENV_BOOTBLOCK && IS_ENABLED(CONFIG_VBOOT_SEPARATE_VERSTAGE))
			return 0;
		return 1;
	}

	if (IS_ENABLED(CONFIG_VBOOT_STARTS_IN_ROMSTAGE)) {
		if (ENV_BOOTBLOCK)
			return 0;
		return 1;
	}

	return 0;
}

/*
 * vb2_check_recovery_request looks up different components to identify if there
 * is a recovery request and returns appropriate reason code:
 * 1. Checks if recovery mode is initiated by EC. If yes, returns
 * VB2_RECOVERY_RO_MANUAL.
 * 2. Checks if recovery request is present in VBNV and returns the code read
 * from it.
 * 3. Checks recovery request in handoff for stages post-cbmem.
 * 4. For non-CBMEM stages, check if vboot verification is done and look-up
 * selected region to identify if vboot_refence library has requested recovery
 * path. If yes, return the reason code from shared data.
 * 5. If nothing applies, return 0 indicating no recovery request.
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

	/*
	 * Check recovery flag in vboot_handoff for stages post CBMEM coming
	 * online. Since for some stages there is no way to know if cbmem has
	 * already come online, try looking up handoff anyways. If it fails,
	 * flow will fallback to looking up shared data.
	 */
	if (cbmem_possibly_online() &&
	    ((reason = vboot_handoff_get_recovery_reason()) != 0))
		return reason;

	/*
	 * For stages where CBMEM might not be online, identify if vboot
	 * verification is already complete and no slot was selected
	 * i.e. recovery path was requested.
	 */
	if (vboot_possibly_executed() && vb2_logic_executed() &&
	    !vb2_is_slot_selected())
		return vb2_get_recovery_reason_shared_data();

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
	if (cbmem_possibly_online() && vboot_handoff_check_developer_flag())
		return 1;

	return 0;
}

#if IS_ENABLED(CONFIG_VBOOT_NO_BOARD_SUPPORT)
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
