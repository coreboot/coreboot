/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdfam10_sysconf.h>

#include <stdlib.h>
#include "mb_sysconf.h"

/* Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables */
struct mb_sysconf_t mb_sysconf;

/* Here you only need to set value in pci1234 for HT-IO that could be
 * installed or not You may need to preset pci1234 for HTIO board, please
 * refer to src/northbridge/amd/amdfam10/get_sblk_pci1234.c for detail
 */
static u32 pci1234x[] = {
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc, 0x0000ffc,
	0x0000ffc, 0x0000ffc,
	};


/* HT Chain device num, actually it is unit id base of every ht device
 * in chain, assume every chain only have 4 ht device at most
 */

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

static u32 get_bus_conf_done = 0;

static u32 get_hcid(u32 i)
{
	u32 id = 0;
	u32 busn = (sysconf.pci1234[i] >> 12) & 0xff;
	u32 devn = sysconf.hcdn[i] & 0xff;
	struct device *dev;

	dev = dev_find_slot(busn, PCI_DEVFN(devn,0));

	switch (dev->device) {
	case 0x7458: /* 8132 */
		id = 1;
		break;
	case 0x7454: /* 8151 */
		id = 2;
		break;
	case 0x7450: /* 8131 */
		id = 3;
		break;
	}
	/* we may need more way to find out hcid: subsystem id? GPIO read ? */
	/* we need use id for 1. bus num, 2. mptable, 3. ACPI table */
	return id;
}

void get_bus_conf(void)
{
	u32 apicid_base;

	struct device *dev;
	int i, j;
	struct mb_sysconf_t *m;

	if(get_bus_conf_done == 1)
		return; /* do it only once */

	get_bus_conf_done = 1;

	sysconf.mb = &mb_sysconf;

	m = sysconf.mb;

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for(i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] >> 8) & 0xff;
	m->sbdn3 = sysconf.hcdn[0] & 0xff;

	m->bus_8132_0 = (sysconf.pci1234[0] >> 12) & 0xff;
	m->bus_8111_0 = m->bus_8132_0;

	/* 8111 */
	dev = dev_find_slot(m->bus_8111_0, PCI_DEVFN(sysconf.sbdn,0));
	if (dev) {
		m->bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8111_0, sysconf.sbdn);
	}

	/* 8132-1 */
	dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3,0));
	if (dev) {
		m->bus_8132_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132_0, m->sbdn3);
	}

	/* 8132-2 */
	dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3+1,0));
	if (dev) {
		m->bus_8132_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132_0, m->sbdn3+1);
	}

	 /* HT chain 1 */
	j = 0;
	for(i = 1; i< sysconf.hc_possible_num; i++) {
		if(!(sysconf.pci1234[i] & 0x1) ) continue;

		/* check hcid type here */
		sysconf.hcid[i] = get_hcid(i);

		switch(sysconf.hcid[i]) {

		case 1:	/* 8132 */
		case 3: /* 8131 */

			m->bus_8132a[j][0] = (sysconf.pci1234[i] >> 12) & 0xff;

			m->sbdn3a[j] = sysconf.hcdn[i] & 0xff;

			/* 8132-1 */
			dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j],0));
			if (dev) {
				m->bus_8132a[j][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			} else {
				printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132a[j][0], m->sbdn3a[j]);
			}

			/* 8132-2 */
			dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j]+1,0));
			if (dev) {
				m->bus_8132a[j][2] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			} else {
				printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8132a[j][0], m->sbdn3a[j]+1);
			}

			break;

		case 2: /* 8151 */

			m->bus_8151[j][0] = (sysconf.pci1234[i] >> 12) & 0xff;
			m->sbdn5[j] = sysconf.hcdn[i] & 0xff;
			/* 8151 */
			dev = dev_find_slot(m->bus_8151[j][0], PCI_DEVFN(m->sbdn5[j]+1, 0));

			if (dev) {
				m->bus_8151[j][1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			} else {
				printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:%02x.0, using defaults\n", m->bus_8151[j][0], m->sbdn5[j]+1);
			}

			break;
		}

		j++;
	 }

/*I/O APICs:	APIC ID	Version	State		Address*/
	apicid_base = 0;
	m->apicid_8111 = apicid_base + 0;
	m->apicid_8132_1 = apicid_base + 1;
	m->apicid_8132_2 = apicid_base + 2;
	for(i = 0; i < j; i++) {
		m->apicid_8132a[i][0] = apicid_base + 3 + i * 2;
		m->apicid_8132a[i][1] = apicid_base + 3 + i * 2 + 1;
	}
}
