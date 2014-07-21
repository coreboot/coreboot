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
#include <cpu/amd/amdfam12.h>
#include "agesawrapper.h"

/* Global variables for MB layouts and these will be shared by irqtable mptable
* and acpi_tables busnum is default.
*/
u8 bus_sb900[6];

void get_bus_conf(void)
{
	device_t dev;
	int i;

	printk(BIOS_DEBUG, "Mainboard - %s - %s - Start.\n", __FILE__, __func__);


	memset(bus_sb900, 0, sizeof(bus_sb900));



	/* sb900 */
	dev = dev_find_slot(0, PCI_DEVFN(0x14, 4));

	if (dev) {
		bus_sb900[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(0,
				  PCI_DEVFN(0x14, i));
		if (dev) {
			bus_sb900[2 + i] =
			    pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
	}

	printk(BIOS_DEBUG, "Mainboard - %s - %s - End.\n", __FILE__, __func__);
}
