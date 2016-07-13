/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2016 Intel Corp.
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

#include <arch/acpi.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <soc/pm.h>
#include "ec.h"

void mainboard_smi_sleep(u8 slp_typ)
{
	if (!IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		return;

	switch (slp_typ) {
	case ACPI_S3:
		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
		enable_gpe(GPIO_TIER_1_SCI);
		break;
	case ACPI_S5:
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
}

int mainboard_smi_apmc(u8 apmc)
{
	if (!IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		return 0;

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
	return 0;
}
