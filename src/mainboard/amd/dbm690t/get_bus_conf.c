/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS==1
#include <cpu/amd/dualcore.h>
#endif

#include <cpu/amd/amdk8_sysconf.h>

/* Global variables for MB layouts and these will be shared by irqtable mptable 
* and acpi_tables busnum is default.
*/
u8 bus_isa;
u8 bus_rs690[8];
u8 bus_sb600[2];
unsigned long apicid_sb600;

/*
* Here you only need to set value in pci1234 for HT-IO that could be installed or not
* You may need to preset pci1234 for HTIO board,
* please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
*/
unsigned long pci1234x[] = {
	0x0000ff0,
};

/*
* HT Chain device num, actually it is unit id base of every ht device in chain,
* assume every chain only have 4 ht device at most
*/
unsigned long hcdnx[] = {
	0x20202020,
};

unsigned long bus_type[256];

unsigned long sbdn_rs690;
unsigned long sbdn_sb600;

extern void get_sblk_pci1234(void);

static unsigned long get_bus_conf_done = 0;

void get_bus_conf(void)
{
	unsigned long apicid_base;
	device_t dev;
	int i, j;

	if (get_bus_conf_done == 1)
		return;		/* do it only once */
	get_bus_conf_done = 1;

	sysconf.hc_possible_num = sizeof(pci1234x) / sizeof(pci1234x[0]);
	for (i = 0; i < sysconf.hc_possible_num; i++) {
		sysconf.pci1234[i] = pci1234x[i];
		sysconf.hcdn[i] = hcdnx[i];
	}

	get_sblk_pci1234();

	sysconf.sbdn = (sysconf.hcdn[0] & 0xff);
	sbdn_rs690 = sysconf.sbdn;
	sbdn_sb600 = 0;

	for (i = 0; i < 2; i++) {
		bus_sb600[i] = 0;
	}
	for (i = 0; i < 8; i++) {
		bus_rs690[i] = 0;
	}

	for (i = 0; i < 256; i++) {
		bus_type[i] = 0; /* default ISA bus. */
	}

	bus_type[0] = 1;	/* pci */

	bus_rs690[0] = (sysconf.pci1234[0] >> 16) & 0xff;
	bus_sb600[0] = bus_rs690[0];

	bus_type[bus_rs690[0]] = 1;

	/* sb600 */
	dev = dev_find_slot(bus_sb600[0], PCI_DEVFN(sbdn_sb600 + 0x14, 4));
	if (dev) {
		bus_sb600[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
		for (j = bus_sb600[1]; j < bus_isa; j++)
			bus_type[j] = 1;
	}

	/* rs690 */
	for (i = 1; i < 8; i++) {
		dev = dev_find_slot(bus_rs690[0], PCI_DEVFN(sbdn_rs690 + i, 0));
		if (dev) {
			bus_rs690[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			if(255 != bus_rs690[i]) {
				bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
				bus_isa++;
				bus_type[bus_rs690[i]] = 1; /* PCI bus. */
			}
		}
	}

	/* I/O APICs:   APIC ID Version State   Address */
	bus_isa = 10;
#if CONFIG_LOGICAL_CPUS==1
	apicid_base = get_apicid_base(1);
#else
	apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
#endif
	apicid_sb600 = apicid_base + 0;
}
