/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <ec/google/chromeec/ec.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/smm.h>
#include "ec.h"
#include "gpio.h"

int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		smm_get_gnvs()->smif = 0;
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	return 1;
}

static u8 mainboard_smi_ec(void)
{
	u8 cmd = 0;
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	u32 pm1_cnt;
	cmd = google_chromeec_get_event();

	/* Log this event */
	if (IS_ENABLED(CONFIG_ELOG_GSMI) && cmd)
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, cmd);

	switch (cmd) {
	case EC_HOST_EVENT_LID_CLOSED:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, ACPI_BASE_ADDRESS + PM1_CNT);
		break;
	}
#endif
	return cmd;
}

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	if (gpi_status_get(sts, EC_SMI_GPI)) {
		/* Process all pending events */
		while (mainboard_smi_ec() != 0)
			;
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	switch (slp_typ) {
	case 3:
		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
		break;
	case 5:
		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S5_WAKE_EVENTS);
		break;
	}

	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	while (google_chromeec_get_event() != 0)
		;
#endif
}

int mainboard_smi_apmc(u8 apmc)
{
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		google_chromeec_set_smi_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0)
			;
		google_chromeec_set_sci_mask(MAINBOARD_EC_SCI_EVENTS);
		break;
	case APM_CNT_ACPI_DISABLE:
		google_chromeec_set_sci_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0)
			;
		google_chromeec_set_smi_mask(MAINBOARD_EC_SMI_EVENTS);
		break;
	}
#endif
	return 0;
}
