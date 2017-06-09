/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <arch/io.h>
#include <cpu/x86/smm.h>
#include <intelblocks/smihandler.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

int smihandler_disable_busmaster(device_t dev)
{
	if (dev == PCH_DEV_PMC)
		return 0;
	return 1;
}

const struct smm_save_state_ops *get_smm_save_state_ops(void)
{
	return &em64t100_smm_ops;
}

void __attribute__((weak))
mainboard_smi_gpi_handler(const struct gpi_status *sts) { }

static void southbridge_smi_gpi(
	const struct smm_save_state_ops *save_state_ops)
{
	struct gpi_status smi_sts;

	gpi_clear_get_smi_status(&smi_sts);
	mainboard_smi_gpi_handler(&smi_sts);

	/* Clear again after mainboard handler */
	gpi_clear_get_smi_status(&smi_sts);
}

const smi_handler_t southbridge_smi[32] = {
	[SLP_SMI_STS] = smihandler_southbridge_sleep,
	[APM_SMI_STS] = smihandler_southbridge_apmc,
	[FAKE_PM1_SMI_STS] = smihandler_southbridge_pm1,
	[GPIO_SMI_STS] = southbridge_smi_gpi,
	[TCO_SMI_STS] = smihandler_southbridge_tco,
	[PERIODIC_SMI_STS] = smihandler_southbridge_periodic,
};
