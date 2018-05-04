/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
 * Copyright (C) 2008 Jonathan A. Kollasch <jakllsch@kollasch.net>
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
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdk8_sysconf.h>
#include <stdlib.h>

/* Global variables for MB layouts and these will be shared by irqtable,
 * mptable and acpi_tables.
 */
/* busnum is default */
unsigned char bus_ck804[6];
unsigned apicid_ck804;

unsigned pci1234x[] = {		//Here you only need to set value in pci1234 for HT-IO that could be installed or not
	//You may need to preset pci1234 for HTIO board, please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
	0x0000ff0,		//no HTIO for ms7135
};
unsigned hcdnx[] = {		//HT Chain device num, actually it is unit id base of every ht device in chain, assume every chain only have 4 ht device at most
	0x20202020,		//ms7135 has only one ht-chain
};

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{
	unsigned apicid_base;

	struct device *dev;
	unsigned sbdn;
	int i;

	if (get_bus_conf_done == 1)
		return;		//do it only once

	get_bus_conf_done = 1;

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for (i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_sblk_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);	// first byte of first chain
	sbdn = sysconf.sbdn;

	for (i = 0; i < 6; i++) {
		bus_ck804[i] = 0;
	}

	bus_ck804[0] = (sysconf.pci1234[0] >> 16) & 0xff;

	/* CK804 */
	int dn = -1;
	for (i = 1; i < 4; i++) {
		switch (i) {
			case 1: dn = 9; break;
			case 2: dn = 13; break;
			case 3: dn = 14; break;
			default: dn = -1; break;
		}
		dev = dev_find_slot(bus_ck804[0], PCI_DEVFN(sbdn + dn, 0));
		if (dev) {
			bus_ck804[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
	}

/*I/O APICs:	APIC ID	Version	State		Address*/
	if (IS_ENABLED(CONFIG_LOGICAL_CPUS))
		apicid_base = get_apicid_base(3);
	else
		apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
	apicid_ck804 = apicid_base + 0;
}
