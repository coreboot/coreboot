/*
 * This file is part of the coreboot project.
 *
 * Written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *  (C) 2005 Digital Design Corporation
 *
 * Ported to Intel XE7501DEVKIT by Agami Aruma
 * Ported to AOpen DXPL Plus-U by Kyösti Mälkki
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
#include <device/pci.h>
#include <device/pci_ids.h>
#include <assert.h>
#include "bus.h"

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int irq_start = 0;
	struct device *dev = NULL;
	struct resource* res = NULL;

	/* SJM: Hard-code CPU LAPIC entries for now */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 0);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 1, 6);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 2, 1);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 3, 7);

	/* Southbridge IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_ICH4, 0xfec00000, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;

	/* P64H2 Bus B IOAPIC */
	dev = dev_find_slot(PCI_BUS_E7501_HI_B, PCI_DEVFN(28, 0));
	if (!dev)
		BUG();		/* Config.lb error? */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_P64H2_BUS_B, res->base, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;

	/* P64H2 Bus A IOAPIC */
	dev = dev_find_slot(PCI_BUS_E7501_HI_B, PCI_DEVFN(30, 0));
	if (!dev)
		BUG();		/* Config.lb error? */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_P64H2_BUS_A, res->base, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;


	/* Map ISA IRQ 0 to IRQ 2 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 1, 0, 2, 0);

	/* IRQ9 differs from ISA standard - ours is active high, level-triggered */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 0, 9, 9, 0xD);

	return current;
}
