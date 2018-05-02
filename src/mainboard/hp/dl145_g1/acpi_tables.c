/*
 * Island Aruma ACPI support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *
 *  Copyright 2005 AMD
 *  2005.9 yhlu modify that to more dynamic for AMD Opteron Based MB
 *
 * Copyright (C) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 * Modified to work with hp/dl145_g1
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "northbridge/amd/amdk8/acpi.h"
#include "mb_sysconf.h"
#include <cpu/amd/powernow.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base = 0x18;

	struct mb_sysconf_t *m;

	get_bus_conf();

	m = sysconf.mb;

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write 8111 IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8111,
			IO_APIC_ADDR, 0);

	/* Write all 8131 IOAPICs */
	{
		device_t dev;
		struct resource *res;
		dev = dev_find_slot(m->bus_8131_0, PCI_DEVFN((sysconf.hcdn[0]&0xff), 1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8131_1,
					res->base, gsi_base );
				gsi_base+=4;

			}
		}
		dev = dev_find_slot(m->bus_8131_0, PCI_DEVFN((sysconf.hcdn[0] & 0xff)+1, 1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8131_2,
					res->base, gsi_base );
				gsi_base+=4;
			}
		}


	}

	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 0, 0, 2, 5 );
		/* 0: mean bus 0--->ISA */
		/* 0: PIC 0 */
		/* 2: APIC 2 */
		/* 5 mean: 0101 --> Edge-triggered, Active high*/


		/* create all subtables for processors */
	current = acpi_create_madt_lapic_nmis(current, 5, 1);
		/* 1: LINT1 connect to NMI */


	return current;
}
