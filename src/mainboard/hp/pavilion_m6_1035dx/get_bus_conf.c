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
u8 bus_isa;
u8 bus_hudson[6];
u32 apicid_hudson;

/*
 * Here you only need to set value in pci1234 for HT-IO that could be installed or not
 * You may need to preset pci1234 for HTIO board,
 * please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
 */
u32 pci1234x[] = {
	0x0000ff0,
};

u32 bus_type[256];
u32 sbdn_hudson;

static u32 get_bus_conf_done = 0;

#if CONFIG_HAVE_ACPI_RESUME
extern u8 acpi_slp_type;
#endif
void get_bus_conf(void)
{
	u32 apicid_base;
	u32 status;

	device_t dev;
	int i, j;

	if (get_bus_conf_done == 1)
		return;   /* do it only once */

	get_bus_conf_done = 1;

	/*
	 * This is the call to AmdInitLate.  It is really in the wrong place, conceptually,
	 * but functionally within the coreboot model, this is the best place to make the
	 * call.  The logically correct place to call AmdInitLate is after PCI scan is done,
	 * after the decision about S3 resume is made, and before the system tables are
	 * written into RAM.  The routine that is responsible for writing the tables is
	 * "write_tables", called near the end of "main".  There is no platform
	 * specific entry point between the S3 resume decision point and the call to
	 * "write_tables", and the next platform specific entry points are the calls to
	 * the ACPI table write functions.  The first of ose would seem to be the right
	 * place, but other table write functions, e.g. the PIRQ table write function, are
	 * called before the ACPI tables are written.  This routine is called at the beginning
	 * of each of the write functions called prior to the ACPI write functions, so this
	 * becomes the best place for this call.
	 */
#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_slp_type != 3) {
		status = agesawrapper_amdinitlate();
		if(status) {
			printk(BIOS_DEBUG, "agesawrapper_amdinitlate failed: %x \n", status);
		}
		status = agesawrapper_amdS3Save();
		if (status) {
			printk(BIOS_DEBUG, "agesawrapper_amds3save failed: %x \n", status);
		}
	}
#else
	status = agesawrapper_amdinitlate();
	if (status)
		printk(BIOS_DEBUG, "agesawrapper_amdinitlate failed: %x \n", status);
#endif

	sbdn_hudson = 0;

	memset(bus_hudson, 0, sizeof(bus_hudson));

	for (i = 0; i < 256; i++) {
		bus_type[i] = 0; /* default ISA bus. */
	}

	bus_type[0] = 1;  /* pci */

	bus_hudson[0] = (pci1234x[0] >> 16) & 0xff;

	/* Hudson */
	dev = dev_find_slot(bus_hudson[0], PCI_DEVFN(sbdn_hudson + 0x14, 4));

	if (dev) {
		bus_hudson[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
		for (j = bus_hudson[1]; j < bus_isa; j++)
			bus_type[j] = 1;
	}

	for (i = 0; i < 4; i++) {
		dev = dev_find_slot(bus_hudson[0], PCI_DEVFN(sbdn_hudson + 0x14, i));
		if (dev) {
			bus_hudson[2 + i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
	}
	for (j = bus_hudson[2]; j < bus_isa; j++)
		bus_type[j] = 1;

	/* I/O APICs:   APIC ID Version State   Address */
	bus_isa = 10;
	apicid_base = CONFIG_MAX_CPUS;
	apicid_hudson = apicid_base;

#if IS_ENABLED(CONFIG_HUDSON_IMC_FWM)
	/* AMD AGESA does not enable thermal zone, so we enable it here. */
	enable_imc_thermal_zone();
#endif
}
