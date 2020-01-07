/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/variants.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/smm.h>
#include <intelblocks/smihandler.h>
#include <variant/ec.h>

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
	/* TODO: Process SMI events from GPI */
}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_sleep_gpio_table(&num);
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
