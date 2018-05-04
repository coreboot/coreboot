/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include <stdlib.h>
#include <cpu/amd/multicore.h>
#include <cpu/amd/amdfam10_sysconf.h>
#if IS_ENABLED(CONFIG_AMD_SB_CIMX)
#include <sb_cimx.h>
#endif

/* Global variables for MB layouts and these will be shared by irqtable mptable
* and acpi_tables busnum is default.
*/
int bus_isa;
u8 bus_rs780[11];
u8 bus_sb800[6];
u32 apicid_sb800;

/*
* Here you only need to set value in pci1234 for HT-IO that could be installed or not
* You may need to preset pci1234 for HTIO board,
* please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
*/
u32 pci1234x[] = {
	0x0000ff0,
};

/*
* HT Chain device num, actually it is unit id base of every ht device in chain,
* assume every chain only have 4 ht device at most
*/
u32 hcdnx[] = {
	0x20202020,
};


u32 sbdn_rs780;
u32 sbdn_sb800;

extern void get_pci1234(void);

static u32 get_bus_conf_done = 0;

void get_bus_conf(void)
{
	u32 apicid_base;
	struct device *dev;
	int i;

	if (get_bus_conf_done == 1)
		return;		/* do it only once */
	get_bus_conf_done = 1;

	sysconf.hc_possible_num = ARRAY_SIZE(pci1234x);
	for (i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);
	sbdn_rs780 = sysconf.sbdn;
	sbdn_sb800 = 0;

	memset(bus_sb800, 0, sizeof(bus_sb800));

	for (i = 0; i < ARRAY_SIZE(bus_rs780); i++) {
		bus_rs780[i] = 0;
	}


	bus_rs780[0] = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_sb800[0] = bus_rs780[0];


	/* sb800 */
	dev = dev_find_slot(bus_sb800[0], PCI_DEVFN(sbdn_sb800 + 0x14, 4));
	if (dev) {
		bus_sb800[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(bus_sb800[0], PCI_DEVFN(sbdn_sb800 + 0x15, i));
		if (dev) {
			bus_sb800[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
	}

	/* rs780 */
	for (i = 1; i < ARRAY_SIZE(bus_rs780); i++) {
		dev = dev_find_slot(bus_rs780[0], PCI_DEVFN(sbdn_rs780 + i, 0));
		if (dev) {
			bus_rs780[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			if(255 != bus_rs780[i]) {
				bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
				bus_isa++;
			}
		}
	}

	/* I/O APICs:   APIC ID Version State   Address */
	bus_isa = 10;
	if (IS_ENABLED(CONFIG_LOGICAL_CPUS))
		apicid_base = get_apicid_base(1);
	else
		apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
	apicid_sb800 = apicid_base + 0;

#if IS_ENABLED(CONFIG_AMD_SB_CIMX)
	sb_Late_Post();
#endif
}
