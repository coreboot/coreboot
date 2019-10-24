/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
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
#include <string.h>
#include <stdint.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdfam10_sysconf.h>

#include <stdlib.h>
#include "mb_sysconf.h"

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
struct mb_sysconf_t mb_sysconf;

void get_bus_conf(void)
{

	unsigned int apicid_base;

	struct device *dev;
	int i;
	struct mb_sysconf_t *m;


	sysconf.mb = &mb_sysconf;

	m = sysconf.mb;
	memset(m, 0, sizeof(struct mb_sysconf_t));

	get_default_pci1234(32);

	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	m->sbdn2 = sysconf.hcdn[0] & 0xff; // bcm5780

	m->bus_bcm5785_0 = (sysconf.pci1234[0] >> 12) & 0xff;
	m->bus_bcm5780[0] = m->bus_bcm5785_0;

		/* bcm5785 */
	printk(BIOS_DEBUG, "search for def %d.0 on bus %d\n",sysconf.sbdn,m->bus_bcm5785_0);
	dev = dev_find_slot(m->bus_bcm5785_0, PCI_DEVFN(sysconf.sbdn,0));
	if (dev) {
		printk(BIOS_DEBUG, "found dev %s...\n",dev_path(dev));
		m->bus_bcm5785_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		printk(BIOS_DEBUG, "secondary is %d...\n",m->bus_bcm5785_1);
		dev = dev_find_slot(m->bus_bcm5785_1, PCI_DEVFN(0xd,0));
		printk(BIOS_DEBUG, "now found %s...\n",dev_path(dev));
		if (dev)
			m->bus_bcm5785_1_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
	else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_bcm5785_0, sysconf.sbdn);
	}

		/* bcm5780 */
	for (i = 1; i < 6; i++) {
		dev = dev_find_slot(m->bus_bcm5780[0], PCI_DEVFN(m->sbdn2 + i - 1,0));
		if (dev)
			m->bus_bcm5780[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		else
			printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_bcm5780[0], m->sbdn2+i-1);
	}

/*I/O APICs:	APIC ID	Version	State		Address*/
	apicid_base = 0x10;
	for (i = 0; i < 3; i++)
		m->apicid_bcm5785[i] = apicid_base+i;
}
