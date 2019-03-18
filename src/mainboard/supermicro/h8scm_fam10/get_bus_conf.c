/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <device/pci.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <stdlib.h>
#include <cpu/amd/multicore.h>

#include <cpu/amd/amdfam10_sysconf.h>

/* Global variables for MB layouts and these will be shared by irqtable mptable
* and acpi_tables busnum is default.
*/
u8 bus_sr5650[14];

void get_bus_conf(void)
{
	struct device *dev;
	int i;

	get_default_pci1234(1);

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);

	for (i = 0; i < ARRAY_SIZE(bus_sr5650); i++) {
		bus_sr5650[i] = 0;
	}


	bus_sr5650[0] = (sysconf.pci1234[0] >> 16) & 0xff;
	pirq_router_bus = bus_sr5650[0];

	/* sr5650 */
	for (i = 1; i < ARRAY_SIZE(bus_sr5650); i++) {
		dev = dev_find_slot(bus_sr5650[0], PCI_DEVFN(sysconf.sbdn + i, 0));
		if (dev) {
			bus_sr5650[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			if(255 != bus_sr5650[i]) {
			}
		}
	}
}
