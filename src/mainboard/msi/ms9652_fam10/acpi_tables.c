/*
 * This file is part of the coreboot project.
 *
 * Written by Stefan Reinauer <stepan@openbios.org>.
 * ACPI FADT, FACS, and DSDT table support added by
 *
 * Copyright (C) 2004 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 Harald Gutmann <harald.gutmann@gmx.net>
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

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <cpu/amd/powernow.h>
#include <device/pci.h>
#include <cpu/amd/amdfam10_sysconf.h>
#include "mb_sysconf.h"

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base = 0x18;
	struct mb_sysconf_t *m;
	//extern unsigned char bus_mcp55[8];
	//extern unsigned apicid_mcp55;

	unsigned sbdn;
	struct resource *res;
	struct device *dev;

	get_bus_conf();
	sbdn = sysconf.sbdn;
	m = sysconf.mb;

	/* Create all subtables for processors. */
	current = acpi_create_madt_lapics(current);

	/* Write SB IOAPIC. */
	dev = dev_find_slot(m->bus_mcp55[0], PCI_DEVFN(sbdn+ 0x1,0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_1);
		if (res) {
			current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				m->apicid_mcp55, res->base,  0);
		}
	}

	/* Write NB IOAPIC. */
	dev = dev_find_slot(m->bus_mcp55[0], PCI_DEVFN(sbdn+ 0x12,1));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_1);
		if (res) {
			current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				m->apicid_mcp55++, res->base,  gsi_base);
		}
	}

	/* IRQ9 ACPI active low. */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	/* IRQ0 -> APIC IRQ2. */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 0, 2, 0x0);

	/* Create all subtables for processors. */
	current = acpi_create_madt_lapic_nmis(current,
		MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);

	return current;
}
