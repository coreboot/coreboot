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

#include <arch/hlt.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <device/pci_def.h>
#include <elog.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/intel/common/smi.h>
#include <spi-generic.h>
#include <stdint.h>
#include <stdlib.h>

int smm_disable_busmaster(device_t dev)
{
	if (dev == PMC_DEV)
		return 0;
	return 1;
}

const struct smm_save_state_ops *get_smm_save_state_ops(void)
{
	return &em64t100_smm_ops;
}

const smi_handler_t southbridge_smi[32] = {
	[SLP_SMI_STS] = southbridge_smi_sleep,
	[APM_SMI_STS] = southbridge_smi_apmc,
	[TCO_SMI_STS] = southbridge_smi_tco,
	[PERIODIC_SMI_STS] = southbridge_smi_periodic,
};
