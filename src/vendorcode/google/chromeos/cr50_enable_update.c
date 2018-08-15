/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <bootstate.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <halt.h>
#include <security/tpm/tss.h>
#include <vb2_api.h>
#include <security/vboot/vboot_common.h>

static void enable_update(void *unused)
{
	int ret;
	uint8_t num_restored_headers;

	/* Nothing to do on recovery mode. */
	if (vboot_recovery_mode_enabled())
		return;

	ret = tlcl_lib_init();

	if (ret != VB2_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed for CR50 update: %x\n",
			ret);
		return;
	}

	/* Reboot in 1000 ms if necessary. */
	ret = tlcl_cr50_enable_update(1000, &num_restored_headers);

	if (ret != TPM_SUCCESS) {
		printk(BIOS_ERR, "Attempt to enable CR50 update failed: %x\n",
			ret);
		return;
	}

	/* If no headers were restored there is no reset forthcoming. */
	if (!num_restored_headers)
		return;

	elog_add_event(ELOG_TYPE_CR50_UPDATE);

	/* clear current post code avoid chatty eventlog on subsequent boot*/
	post_code(0);

	printk(BIOS_INFO, "Waiting for CR50 reset to pick up update.\n");

	if (IS_ENABLED(CONFIG_POWER_OFF_ON_CR50_UPDATE)) {
		if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC) &&
		    !IS_ENABLED(CONFIG_DISABLE_POWER_OFF_EC_ON_CR50_UPDATE)) {
			printk(BIOS_INFO, "Hibernating EC. Clearing AP_OFF.\n");
			google_chromeec_reboot(0,
					       EC_REBOOT_HIBERNATE_CLEAR_AP_OFF,
					       EC_REBOOT_FLAG_ON_AP_SHUTDOWN);
		}
		poweroff();
	}
	halt();
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, enable_update, NULL);
