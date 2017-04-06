/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Siemens AG
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

#include <delay.h>
#include <types.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <device/pci_def.h>
#include <cpu/x86/smm.h>
#include <spi-generic.h>
#include <elog.h>
#include <halt.h>
#include <pc80/mc146818rtc.h>
#include <soc/lpc.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/smm.h>


/**
 * @brief Set the EOS bit
 */
void southbridge_smi_set_eos(void)
{
	enable_smi(EOS);
}

static void southbridge_smi_serirq(void)
{

}

typedef void (*smi_handler_t)(void);

static smi_handler_t southbridge_smi[32] = {
	NULL,			  //  [0] reserved
	NULL,			  //  [1] reserved
	NULL,			  //  [2] BIOS_STS
	NULL,			  //  [3] LEGACY_USB_STS
	NULL,			  //  [4] SLP_SMI_STS
	NULL,			  //  [5] APM_STS
	NULL,			  //  [6] SWSMI_TMR_STS
	NULL,			  //  [7] reserved
	NULL,			  //  [8] PM1_STS
	NULL,			  //  [9] GPE0_STS
	NULL,			  // [10] GPI_STS
	NULL,			  // [11] MCSMI_STS
	NULL,			  // [12] DEVMON_STS
	NULL,			  // [13] TCO_STS
	NULL,			  // [14] PERIODIC_STS
	southbridge_smi_serirq,	  // [15] SERIRQ_SMI_STS
	NULL,			  // [16] SMBUS_SMI_STS
	NULL,			  // [17] LEGACY_USB2_STS
	NULL,			  // [18] INTEL_USB2_STS
	NULL,			  // [19] reserved
	NULL,			  // [20] PCI_EXP_SMI_STS
	NULL,			  // [21] MONITOR_STS
	NULL,			  // [22] reserved
	NULL,			  // [23] reserved
	NULL,			  // [24] reserved
	NULL,			  // [25] EL_SMI_STS
	NULL,			  // [26] SPI_STS
	NULL,			  // [27] reserved
	NULL,			  // [28] reserved
	NULL,			  // [29] reserved
	NULL,			  // [30] reserved
	NULL			  // [31] reserved
};

/**
 * @brief Interrupt handler for SMI#
 *
 * @param smm_revision revision of the smm state save map
 */

void southbridge_smi_handler(void)
{
	int i;
	u32 smi_sts;

	/* We need to clear the SMI status registers, or we won't see what's
	   happening in the following calls. */
	smi_sts = clear_smi_status();

	/* Call SMI sub handler for each of the status bits */
	for (i = 0; i < 31; i++) {
		if (smi_sts & (1 << i)) {
			if (southbridge_smi[i]) {
				southbridge_smi[i]();
			} else {
				printk(BIOS_DEBUG,
				       "SMI_STS[%d] occurred, but no "
				       "handler available.\n", i);
			}
		}
	}
}
