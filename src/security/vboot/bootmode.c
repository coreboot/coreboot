/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootmode.h>
#include <bootstate.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>

/*
 * Functions which check vboot information should only be called after verstage
 * has run.  Otherwise, they will hit the assertion in vboot_get_context().
 */

int vboot_check_recovery_request(void)
{
	return vb2api_get_recovery_reason(vboot_get_context());
}

int vboot_recovery_mode_enabled(void)
{
	return vboot_get_context()->flags & VB2_CONTEXT_RECOVERY_MODE;
}

int vboot_developer_mode_enabled(void)
{
	return vboot_get_context()->flags & VB2_CONTEXT_DEVELOPER_MODE;
}

int __weak clear_recovery_mode_switch(void)
{
	return 0;
}

static void do_clear_recovery_mode_switch(void *unused)
{
	if (vboot_get_context()->flags & VB2_CONTEXT_FORCE_RECOVERY_MODE)
		clear_recovery_mode_switch();
}
/*
 * The recovery mode switch (typically backed by EC) is not cleared until
 * BS_WRITE_TABLES for two reasons:
 *
 * (1) On some platforms, FSP initialization may cause a reboot.  Push clearing
 * the recovery mode switch until after FSP code runs, so that a manual recovery
 * request (three-finger salute) will function correctly under this condition.
 *
 * (2) To give the implementation of clear_recovery_mode_switch a chance to
 * add an event to elog.  See the function in chromeec/switches.c.
 */
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
		      do_clear_recovery_mode_switch, NULL);

int __weak get_recovery_mode_retrain_switch(void)
{
	return 0;
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

void __weak fill_lb_gpios(struct lb_gpios *gpios)
{
}

#endif
