/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include "agesawrapper.h"

#include <console/console.h>
#include <cpu/amd/amdfam15.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <southbridge/amd/agesa/hudson/imc.h>

/* Global variables for MB layouts and these will be shared by irqtable mptable
 * and acpi_tables busnum is default.
 */
u8 bus_hudson[6];
u32 apicid_hudson;


void get_bus_conf(void)
{
	u32 apicid_base;

	device_t dev;
	int i;


	memset(bus_hudson, 0, sizeof(bus_hudson));


	/* Hudson */
	dev = dev_find_slot(0, PCI_DEVFN(0x14, 4));

	if (dev) {
		bus_hudson[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(0, PCI_DEVFN(0x14, i));
		if (dev) {
			bus_hudson[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
	}

	/* I/O APICs:   APIC ID Version State   Address */
	apicid_base = CONFIG_MAX_CPUS;
	apicid_hudson = apicid_base;

#if IS_ENABLED(CONFIG_HUDSON_IMC_FWM)
	/* AMD AGESA does not enable thermal zone, so we enable it here. */
	enable_imc_thermal_zone();
#endif
}
