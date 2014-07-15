/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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
#include "agesawrapper.h"
#include <cpu/amd/amdfam10_sysconf.h>


/* Global variables for MB layouts and these will be shared by irqtable mptable
 * and acpi_tables busnum is default.
 */
u8 bus_isa;
u8 bus_sp5100[2];
u8 bus_sr5650[14];



u32 sbdn_sr5650;
u32 sbdn_sp5100;

void get_bus_conf(void)
{
	device_t dev;
	int i;

	sbdn_sp5100 = 0;

	for (i = 0; i < ARRAY_SIZE(bus_sp5100); i++) {
		bus_sp5100[i] = 0;
	}
	for (i = 0; i < ARRAY_SIZE(bus_sr5650); i++) {
		bus_sr5650[i] = 0;
	}


	bus_sr5650[0] = 0;
	bus_sp5100[0] = bus_sr5650[0];

	/* sp5100 */
	dev = dev_find_slot(bus_sp5100[0], PCI_DEVFN(sbdn_sp5100 + 0x14, 4));

	if (dev) {
		bus_sp5100[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
	}

	/* sr5650 */
	for (i = 1; i < ARRAY_SIZE(bus_sr5650); i++) {
		dev = dev_find_slot(bus_sr5650[0], PCI_DEVFN(sbdn_sr5650 + i, 0));
		if (dev) {
			bus_sr5650[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			if(255 != bus_sr5650[i]) {
				bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
				bus_isa++;
			}
		}
	}

/*
	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(bus_sp5100[0], PCI_DEVFN(sbdn_sp5100 + 0x14, i));
		if (dev) {
			bus_sp5100[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
	}
*/


	/* I/O APICs:   APIC ID Version State   Address */
	bus_isa = 10;

}
