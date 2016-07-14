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
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <ec/google/chromeec/smm.h>
#include <gpio.h>
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

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	if (gpi_status_get(sts, EC_SMI_GPI))
		chromeec_smi_process_events();
}

static void mainboard_gpio_smi_sleep(u8 slp_typ)
{
	int i;

	/* Power down the rails on any sleep type. */
	gpio_t active_high_signals[] = {
		EN_PP3300_KEPLER,
		EN_PP3300_DX_TOUCH,
		EN_PP3300_DX_EMMC,
		EN_PP1800_DX_EMMC,
		EN_PP3300_DX_CAM,
	};

	for (i = 0; i < ARRAY_SIZE(active_high_signals); i++)
		gpio_set(active_high_signals[i], 0);
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
					MAINBOARD_EC_S5_WAKE_EVENTS);

	mainboard_gpio_smi_sleep(slp_typ);
}

int mainboard_smi_apmc(u8 apmc)
{
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC))
		chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
					MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
