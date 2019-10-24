/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <stdlib.h>
#include <cpu/amd/multicore.h>
#include <cpu/amd/amdfam10_sysconf.h>

/*
 * Global variables for MB layouts and these will be shared by irqtable,
 * mptable and acpi_tables.
 */
/* busnum is default */
unsigned char bus_ck804[6];
unsigned int apicid_ck804;

void get_bus_conf(void)
{
	unsigned int apicid_base, sbdn;
	struct device *dev;
	int i;

	get_default_pci1234(32);

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff); // first byte of first chain
	sbdn = sysconf.sbdn;

	for (i = 0; i < 6; i++)
		bus_ck804[i] = 0;

	/* CK804 */
	dev = dev_find_slot(bus_ck804[0], PCI_DEVFN(sbdn + 0x09, 0));
	if (dev) {
		bus_ck804[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_ck804[2] = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_ck804[2]++;
	} else {
		printk
		    (BIOS_DEBUG, "ERROR - could not find PCI 1:%02x.0, using defaults\n",
		     sbdn + 0x09);
		bus_ck804[1] = 2;
		bus_ck804[2] = 3;
	}

	for (i = 2; i < 6; i++) {
		dev = dev_find_slot(bus_ck804[0],
				    PCI_DEVFN(sbdn + 0x0b + i - 2, 0));
		if (dev) {
			bus_ck804[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n",
			     bus_ck804[0], sbdn + 0x0b + i - 2);
		}
	}

	if (CONFIG(LOGICAL_CPUS)) {
		apicid_base = get_apicid_base(1);
		printk(BIOS_SPEW, "CONFIG_LOGICAL_CPUS == 1: apicid_base: %08x\n", apicid_base);
	}
	else {
		apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
		printk(BIOS_SPEW, "CONFIG_LOGICAL_CPUS == 0: apicid_base: %08x\n", apicid_base);
	}
	apicid_ck804 = apicid_base + 0;
}
