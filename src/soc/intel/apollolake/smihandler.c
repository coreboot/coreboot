/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2020 Intel Corp.
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

#include <cpu/x86/smm.h>
#include <cpu/intel/em64t100_save_state.h>
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

const smi_handler_t southbridge_smi[32] = {
	[SMI_ON_SLP_EN_STS_BIT] = smihandler_southbridge_sleep,
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
	[GPIO_STS_BIT] = smihandler_southbridge_gpi,
	[TCO_STS_BIT] = smihandler_southbridge_tco,
	[PERIODIC_STS_BIT] = smihandler_southbridge_periodic,
#if CONFIG(SOC_ESPI)
	[ESPI_SMI_STS_BIT] = smihandler_southbridge_espi,
#endif
};
