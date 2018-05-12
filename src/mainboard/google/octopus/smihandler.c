/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Intel Corp.
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
#include <baseboard/variants.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/smm.h>
#include <intelblocks/smihandler.h>
#include <soc/pm.h>
#include <soc/gpio.h>
#include <variant/ec.h>
#include <variant/gpio.h>

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	if (gpi_status_get(sts, EC_SMI_GPI))
		chromeec_smi_process_events();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_sleep_gpio_table(&num, slp_typ);
	gpio_configure_pads(pads, num);

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS,
				MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS,
				MAINBOARD_EC_SMI_EVENTS);
	return 0;
}

void mainboard_smi_espi_handler(void)
{
	chromeec_smi_process_events();
}
