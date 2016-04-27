/*
 * This file is part of the coreboot project.
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
 * ACPI support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *
 *  Copyright 2005 AMD
 *  2005.9 yhlu modify that to more dynamic for AMD Opteron Based MB
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 */

#include <console/console.h>
#include <string.h>
#include <assert.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/amdk8_sysconf.h>

/* APIC */
unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *dev;
	struct resource *res;

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write NVIDIA CK804 IOAPIC. */
	dev = dev_find_slot(0x0, PCI_DEVFN(sysconf.sbdn + 0x1, 0));
	ASSERT(dev != NULL);

	res = find_resource(dev, PCI_BASE_ADDRESS_1);
	ASSERT(res != NULL);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
		CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS, res->base, 0);

	/* Initialize interrupt mapping if mptable.c didn't. */
	if (!IS_ENABLED(CONFIG_GENERATE_MP_TABLE)) {
		/* Copied from mptable.c */
		/* Enable interrupts for commonly used devices (USB, SATA, etc.) */
		pci_write_config32(dev, 0x7c, 0x0d800018);
		pci_write_config32(dev, 0x80, 0xd8002009);
		pci_write_config32(dev, 0x84, 0x00000001);
	}

// 	/* IRQ of timer (override IRQ0 --> APIC IRQ2) */
// 	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
// 		current, 0, 0, 2, 0);
	/* IRQ9 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_HIGH);
	/* IRQ14 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 14, 14, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH);
	/* IRQ15 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 15, 15, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH);

	/* create all subtables for processors */
	/* acpi_create_madt_lapic_nmis returns current, not size. */
	current = acpi_create_madt_lapic_nmis(current,
			MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);

	return current;
}
