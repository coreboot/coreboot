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

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <cpu/amd/amdfam14.h>
#include "agesawrapper.h"
#if IS_ENABLED(CONFIG_HUDSON_IMC_FWM)
#include "imc.h"
#endif

/* Global variables for MB layouts and these will be shared by irqtable mptable
 * and acpi_tables busnum is default.
 */
u8 bus_isa;
u8 bus_yangtze[6];
u32 apicid_yangtze;

/*
 * Here you only need to set value in pci1234 for HT-IO that could be installed or not
 * You may need to preset pci1234 for HTIO board,
 * please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
 */
u32 pci1234x[] = {
	0x0000ff0,
};

u32 bus_type[256];
u32 sbdn_yangtze;


void get_bus_conf(void)
{
	u32 apicid_base;
	u32 value;

	device_t dev;
	int i, j;


	dev = dev_find_slot(0, PCI_DEVFN(0, 0)); /* clear IoapicSbFeatureEn */
	pci_write_config32(dev, 0xF8, 0);
	pci_write_config32(dev, 0xFC, 5); /* TODO: move it to dsdt.asl */

	/* disable No Snoop */
	dev = dev_find_slot(0, PCI_DEVFN(1, 1));
	value = pci_read_config32(dev, 0x60);
	value &= ~(1 << 11);
	pci_write_config32(dev, 0x60, value);

	sbdn_yangtze = 0;

	memset(bus_yangtze, 0, sizeof(bus_yangtze));

	for (i = 0; i < 256; i++) {
		bus_type[i] = 0; /* default ISA bus. */
	}

	bus_type[0] = 1;  /* pci */

	//  bus_yangtze[0] = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_yangtze[0] = (pci1234x[0] >> 16) & 0xff;

	/* yangtze */
	dev = dev_find_slot(bus_yangtze[0], PCI_DEVFN(sbdn_yangtze + 0x14, 4));

	if (dev) {
		bus_yangtze[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
		for (j = bus_yangtze[1]; j < bus_isa; j++)
			bus_type[j] = 1;
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(bus_yangtze[0], PCI_DEVFN(sbdn_yangtze + 0x14, i));
		if (dev) {
			bus_yangtze[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
	}
	for (j = bus_yangtze[2]; j < bus_isa; j++)
		bus_type[j] = 1;

	/* I/O APICs:   APIC ID Version State   Address */
	bus_isa = 10;
	apicid_base = CONFIG_MAX_CPUS;
	apicid_yangtze = apicid_base;

#if IS_ENABLED(CONFIG_HUDSON_IMC_FWM)
	/* AMD AGESA does not enable thermal zone, so we enable it here. */
	enable_imc_thermal_zone();
#endif
}
