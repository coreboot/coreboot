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


/* Global variables for MB layouts and these will be shared by irqtable mptable
 * and acpi_tables busnum is default.
 */
u8 bus_sp5100[2];

void get_bus_conf(void);

void get_bus_conf(void)
{
	device_t dev;
	int i;


	for (i = 0; i < 0; i++) {
		bus_sp5100[i] = 0;
	}

	bus_sp5100[0] = 0;

	/* sp5100 */
	dev = dev_find_slot(0, PCI_DEVFN(0x14, 4));

	if (dev) {
		bus_sp5100[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
}
