/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2009 Thomas Jourdan <thomas.jourdan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "ioapic.h"

#define IO_APIC0 2
#define IO_APIC1 3

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int irq_start = 0;
	device_t dev = 0;
	unsigned char bus_isa;

	/* Local Apic */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *) current, 1, 0);
	// This one is for the second core... Will it hurt?
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *) current, 2, 1);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, IO_APIC0, IO_APIC_ADDR, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, IO_APIC1, IO_APIC_ADDR + 0x10000, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;

	dev = dev_find_slot(0, PCI_DEVFN(0x1e,0));

	if (dev) {
		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
	} else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1e.0, using defaults\n");
		bus_isa = 7;
	}

	/* Map ISA IRQ 0 to IRQ 2 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current, bus_isa, 0, 2, 0);

	/* IRQ9 differs from ISA standard - ours is active high, level-triggered */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current, 0, 9, 9, 0x000d);

	return current;
}
