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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS
#include <cpu/amd/multicore.h>
#endif

#include <cpu/amd/amdfam10_sysconf.h>

#include <stdlib.h>
#include "mb_sysconf.h"

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
struct mb_sysconf_t mb_sysconf;

/* Here you only need to set value in pci1234 for HT-IO that could be
   installed or not You may need to preset pci1234 for HTIO board, please
   refer to src/northbridge/amd/amdfam10/get_pci1234.c for detail */
static u32 pci1234x[] = {
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc,
};


/* HT Chain device num, actually it is unit id base of every ht device
   in chain, assume every chain only have 4 ht device at most */

static unsigned hcdnx[] = {
	0x20202020, 0x20202020, 0x20202020, 0x20202020, 0x20202020,
	0x20202020, 0x20202020, 0x20202020, 0x20202020, 0x20202020,
	0x20202020, 0x20202020, 0x20202020, 0x20202020, 0x20202020,
	0x20202020, 0x20202020, 0x20202020, 0x20202020, 0x20202020,
	0x20202020, 0x20202020, 0x20202020, 0x20202020, 0x20202020,
	0x20202020, 0x20202020, 0x20202020, 0x20202020, 0x20202020,
	0x20202020, 0x20202020,
};

extern void get_pci1234(void);

static unsigned get_bus_conf_done = 0;

void get_bus_conf(void)
{

	unsigned apicid_base;

	device_t dev;
	int i;
	struct mb_sysconf_t *m;

	if(get_bus_conf_done==1) return; //do it only once

	get_bus_conf_done = 1;

	sysconf.mb = &mb_sysconf;

	m = sysconf.mb;
	memset(m, 0, sizeof(struct mb_sysconf_t));

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);

	for(i=0;i<sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_pci1234();

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
		if(dev) {
			m->bus_bcm5785_1_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
	}
	else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_bcm5785_0, sysconf.sbdn);
	}

		/* bcm5780 */
	for(i = 1; i < 6; i++) {
		dev = dev_find_slot(m->bus_bcm5780[0], PCI_DEVFN(m->sbdn2 + i - 1,0));
		if(dev) {
			m->bus_bcm5780[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_bcm5780[0], m->sbdn2+i-1);
		}
	}

/*I/O APICs:	APIC ID	Version	State		Address*/
	apicid_base = 0x10;
	for(i=0;i<3;i++)
		m->apicid_bcm5785[i] = apicid_base+i;
}
