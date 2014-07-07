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
#include <cpu/amd/amdfam14.h>
#include "agesawrapper.h"


/* Global variables for MB layouts and these will be shared by irqtable mptable
* and acpi_tables busnum is default.
*/
u8 bus_isa;
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

u32 bus_type[256];
u32 sbdn_sb800;

void get_bus_conf(void)
{
	u32 apicid_base;

	device_t dev;
	int i, j;

	sbdn_sb800 = 0;

	memset(bus_sb800, 0, sizeof(bus_sb800));

	for (i = 0; i < 256; i++) {
		bus_type[i] = 0; /* default ISA bus. */
	}

	bus_type[0] = 1;	/* pci */

//	bus_sb800[0] = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_sb800[0] = (pci1234x[0] >> 16) & 0xff;

	/* sb800 */
	dev = dev_find_slot(bus_sb800[0], PCI_DEVFN(sbdn_sb800 + 0x14, 4));

	if (dev) {
		bus_sb800[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
		for (j = bus_sb800[1]; j < bus_isa; j++)
			bus_type[j] = 1;
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(bus_sb800[0], PCI_DEVFN(sbdn_sb800 + 0x14, i));
		if (dev) {
			bus_sb800[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
	}

	for (j = bus_sb800[2]; j < bus_isa; j++)
		bus_type[j] = 1;


	/* I/O APICs:    APIC ID Version State   Address */
	bus_isa = 10;
	apicid_base = CONFIG_MAX_CPUS;
	apicid_sb800 = apicid_base;

}
