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

int smihandler_soc_disable_busmaster(pci_devfn_t dev)
{
	if (dev == PCH_DEV_PMC)
		return 0;
	return 1;
}

const struct smm_save_state_ops *get_smm_save_state_ops(void)
{
	return &em64t100_smm_ops;
}

/* SMI handlers that should be serviced in SCI mode too. */
uint32_t smihandler_soc_get_sci_mask(void)
{
	uint32_t sci_mask =
		SMI_HANDLER_SCI_EN(APM_SMI_STS) |
		SMI_HANDLER_SCI_EN(SLP_SMI_STS);

	return sci_mask;
}

const smi_handler_t southbridge_smi[32] = {
	[SLP_SMI_STS] = smihandler_southbridge_sleep,
	[APM_SMI_STS] = smihandler_southbridge_apmc,
	[FAKE_PM1_SMI_STS] = smihandler_southbridge_pm1,
	[GPIO_SMI_STS] = smihandler_southbridge_gpi,
	[TCO_SMI_STS] = smihandler_southbridge_tco,
	[PERIODIC_SMI_STS] = smihandler_southbridge_periodic,
#if IS_ENABLED(CONFIG_SOC_ESPI)
	[ESPI_SMI_STS_BIT] = smihandler_southbridge_espi,
#endif
};
