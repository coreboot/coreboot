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
#include <ec/google/chromeec/smm.h>
#include <gpio.h>
#include <soc/smi.h>
#include <variant/ec.h>
#include <variant/gpio.h>

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		if (gpi_sts & (1 << EC_SMI_GPI))
			chromeec_smi_process_events();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
					MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
					MAINBOARD_EC_SMI_EVENTS);

	/* Enable backlight - GPIO active low */
	gpio_set(GPIO_133, 0);

	return 0;
}
