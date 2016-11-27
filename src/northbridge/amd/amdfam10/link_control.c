/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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

/* Configure various power control registers, including processor
 * boost support.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <lib.h>
#include <cpu/amd/model_10xxx_rev.h>

#include "amdfam10.h"

static void nb_control_init(struct device *dev)
{
	uint8_t enable_c_states;
	uint8_t enable_cc6;
	uint32_t dword;

	printk(BIOS_DEBUG, "NB: Function 4 Link Control.. ");

	/* Configure L3 Power Control */
	dword = pci_read_config32(dev, 0x1c4);
	dword |= (0x1 << 8);			/* L3PwrSavEn = 1 */
	pci_write_config32(dev, 0x1c4, dword);

	if (is_fam15h()) {
		/* Configure L3 Control 2 */
		dword = pci_read_config32(dev, 0x1cc);
		dword &= ~(0x7 << 6);			/* ImplRdProjDelayThresh = 0x2 */
		dword |= (0x2 << 6);
		pci_write_config32(dev, 0x1cc, dword);

		/* Configure TDP Accumulator Divisor Control */
		dword = pci_read_config32(dev, 0x104);
		dword &= ~(0xfff << 2);			/* TdpAccDivRate = 0xc8 */
		dword |= (0xc8 << 2);
		dword &= ~0x3;				/* TdpAccDivVal = 0x1 */
		dword |= 0x1;
		pci_write_config32(dev, 0x104, dword);

		/* Configure Sample and Residency Timers */
		dword = pci_read_config32(dev, 0x110);
		dword &= ~0xfff;			/* CSampleTimer = 0x1 */
		dword |= 0x1;
		pci_write_config32(dev, 0x110, dword);

		/* Configure APM TDP Control */
		dword = pci_read_config32(dev, 0x16c);
		dword |= (0x1 << 4);			/* ApmTdpLimitIntEn = 1 */
		pci_write_config32(dev, 0x16c, dword);

		/* Enable APM */
		dword = pci_read_config32(dev, 0x15c);
		dword |= (0x1 << 7);			/* ApmMasterEn = 1 */
		pci_write_config32(dev, 0x15c, dword);

		enable_c_states = 0;
		enable_cc6 = 0;
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
		uint8_t nvram;

		if (get_option(&nvram, "cpu_c_states") == CB_SUCCESS)
			enable_c_states = !!nvram;

		if (get_option(&nvram, "cpu_cc6_state") == CB_SUCCESS)
			enable_cc6 = !!nvram;
#endif

		if (enable_c_states) {
			/* Configure C-state Control 1 */
			dword = pci_read_config32(dev, 0x118);
			dword |= (0x1 << 24);		/* PwrGateEnCstAct1 = 1 */
			dword &= ~(0x7 << 21);		/* ClkDivisorCstAct1 = 0x0 */
			dword &= ~(0x3 << 18);		/* CacheFlushTmrSelCstAct1 = 0x1 */
			dword |= (0x1 << 18);
			dword |= (0x1 << 17);		/* CacheFlushEnCstAct1 = 1 */
			dword |= (0x1 << 16);		/* CpuPrbEnCstAct1 = 1 */
			dword &= ~(0x1 << 8);		/* PwrGateEnCstAct0 = 0 */
			dword &= ~(0x7 << 5);		/* ClkDivisorCstAct0 = 0x0 */
			dword &= ~(0x3 << 2);		/* CacheFlushTmrSelCstAct0 = 0x2 */
			dword |= (0x2 << 2);
			dword |= (0x1 << 1);		/* CacheFlushEnCstAct0 = 1 */
			dword |= 0x1;			/* CpuPrbEnCstAct0 = 1 */
			pci_write_config32(dev, 0x118, dword);

			/* Configure C-state Control 2 */
			dword = pci_read_config32(dev, 0x11c);
			dword &= ~(0x1 << 8);		/* PwrGateEnCstAct2 = 0 */
			dword &= ~(0x7 << 5);		/* ClkDivisorCstAct2 = 0x0 */
			dword &= ~(0x3 << 2);		/* CacheFlushTmrSelCstAct0 = 0x0 */
			dword &= ~(0x1 << 1);		/* CacheFlushEnCstAct0 = 0 */
			dword &= ~(0x1);		/* CpuPrbEnCstAct0 = 0 */
			pci_write_config32(dev, 0x11c, dword);

			/* Configure C-state Policy Control 1 */
			dword = pci_read_config32(dev, 0x128);
			dword &= ~(0x7f << 5);		/* CacheFlushTmr = 0x28 */
			dword |= (0x28 << 5);
			dword &= ~0x1;			/* CoreCstateMode = !enable_cc6 */
			dword |= ((enable_cc6)?0:1);
			pci_write_config32(dev, 0x128, dword);
		}
	}

	printk(BIOS_DEBUG, "done.\n");
}


static struct device_operations mcf4_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = nb_control_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static const struct pci_driver mcf4_driver_fam10 __pci_driver = {
	.ops    = &mcf4_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1204,
};

static const struct pci_driver mcf4_driver_fam15_model10 __pci_driver = {
	.ops    = &mcf4_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1404,
};

static const struct pci_driver mcf4_driver_fam15 __pci_driver = {
	.ops    = &mcf4_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1604,
};
