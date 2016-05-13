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
	NULL,			  /*  [0] reserved */
	NULL,			  /*  [1] reserved */
	NULL,			  /*  [2] BIOS_STS */
	NULL,			  /*  [3] LEGACY_USB_STS */
	southbridge_smi_sleep,	  /*  [4] SLP_SMI_STS */
	southbridge_smi_apmc,	  /*  [5] APM_STS */
	NULL,			  /*  [6] SWSMI_TMR_STS */
	NULL,			  /*  [7] reserved */
	southbridge_smi_pm1,	  /*  [8] PM1_STS */
	southbridge_smi_gpe0,	  /*  [9] GPE0_STS */
	NULL,			  /* [10] reserved */
	NULL,			  /* [11] reserved */
	NULL,			  /* [12] reserved */
	southbridge_smi_tco,	  /* [13] TCO_STS */
	southbridge_smi_periodic, /* [14] PERIODIC_STS */
	NULL,			  /* [15] SERIRQ_SMI_STS */
	NULL,			  /* [16] SMBUS_SMI_STS */
	NULL,			  /* [17] LEGACY_USB2_STS */
	NULL,			  /* [18] INTEL_USB2_STS */
	NULL,			  /* [19] reserved */
	NULL,			  /* [20] PCI_EXP_SMI_STS */
	NULL,			  /* [21] reserved */
	NULL,			  /* [22] reserved */
	NULL,			  /* [23] reserved */
	NULL,			  /* [24] reserved */
	NULL,			  /* [25] reserved */
	NULL,			  /* [26] SPI_STS */
	NULL,			  /* [27] reserved */
	NULL,			  /* [28] PUNIT */
	NULL,			  /* [29] GUNIT */
	NULL,			  /* [30] reserved */
	NULL			  /* [31] reserved */
};
