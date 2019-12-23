/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/variants.h>
#include <cpu/x86/smm.h>
#include <intelblocks/smihandler.h>

void mainboard_smi_gpi_handler(const struct gpi_status *sts)
{
}

void mainboard_smi_sleep(u8 slp_typ)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_sleep_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

int mainboard_smi_apmc(u8 apmc)
{
	return 0;
}
