/*
 * This file is part of the coreboot project.
 *
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#if CONFIG_LOGICAL_CPUS
#include <cpu/amd/multicore.h>
#endif
#include <cpu/amd/amdk8_sysconf.h>
#include <stdlib.h>

/*
 * Global variables for MB layouts and these will be shared by irqtable,
 * mptable and acpi_tables.
 */
/* busnum is default */
unsigned char bus_ck804[6];
unsigned apicid_ck804;

/*
 * Here you only need to set value in pci1234 for HT-IO that could be installed
 * or not. You may need to preset pci1234 for HT-IO board, please refer to
 * src/northbridge/amd/amdk8/get_sblk_pci1234.c for details.
 */
unsigned pci1234x[] = {
	0x0000ff0,		/* No HTIO for A8N-E */
};

/*
 * HT Chain device num, actually it is unit id base of every ht device in
 * chain, assume every chain only have 4 ht device at most.
 */
unsigned hcdnx[] = {
	0x20202020,		/* A8N-E has only one ht-chain */
};

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{
	unsigned apicid_base, sbdn;
	device_t dev;
	int i;

	if (get_bus_conf_done == 1)
		return;		/* Do it only once. */

	get_bus_conf_done = 1;

	/* FIXME: Is this really needed twice? */
	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for (i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_sblk_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff); // first byte of first chain
	sbdn = sysconf.sbdn;

	for (i = 0; i < 6; i++)
		bus_ck804[i] = 0;

	bus_ck804[0] = (sysconf.pci1234[0] >> 16) & 0xff;

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

#if CONFIG_LOGICAL_CPUS
	apicid_base = get_apicid_base(3);
#else
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
#endif
	apicid_ck804 = apicid_base + 0;
}
