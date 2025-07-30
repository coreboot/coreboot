/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <fsp/util.h>
#include <timestamp.h>
#include <ux_locales.h>

#include "ux.h"

static bool ux_inform_user_of_operation(const char *name, enum ux_locale_msg id,
		 FSPM_UPD *mupd)
{
	timestamp_add_now(TS_ESOL_START);

	if (!CONFIG(CHROMEOS_ENABLE_ESOL)) {
		timestamp_add_now(TS_ESOL_END);
		return false;
	}

	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	void *vbt;
	size_t vbt_size;
	static bool ux_done = false;

	/*
	 * Prevents multiple VGA text messages from being rendered during the boot process.
	 *
	 * This function is designed to be called only once. Subsequent calls are intentionally
	 * ignored to avoid overwriting previously displayed messages. For example, if a
	 * low-battery shutdown notification is scheduled, a later call with a firmware
	 * update notification could result in the low-battery message being lost.
	 */
	if (ux_done) {
		timestamp_add_now(TS_ESOL_END);
		return true;
	}

	printk(BIOS_INFO, "Informing user on-display of %s.\n", name);

	vbt = cbfs_map("vbt.bin", &vbt_size);
	if (!vbt) {
		printk(BIOS_ERR, "Could not load vbt.bin\n");
		return false;
	}

	m_cfg->VgaInitControl = VGA_INIT_CONTROL_ENABLE;
	m_cfg->VbtPtr = (efi_uintn_t)vbt;
	m_cfg->VbtSize = vbt_size;
	m_cfg->LidStatus = CONFIG(VBOOT_LID_SWITCH) ? get_lid_switch() : CONFIG(RUN_FSP_GOP);
	m_cfg->VgaMessage = (efi_uintn_t)ux_locales_get_text(id);

	ux_done = true;

	timestamp_add_now(TS_ESOL_END);
	return true;
}

bool ux_inform_user_of_update_operation(const char *name, FSPM_UPD *mupd)
{
	return ux_inform_user_of_operation(name, UX_LOCALE_MSG_MEMORY_TRAINING, mupd);
}

bool ux_inform_user_of_poweroff_operation(const char *name, FSPM_UPD *mupd)
{
	return ux_inform_user_of_operation(name, UX_LOCALE_MSG_LOW_BATTERY, mupd);
}
