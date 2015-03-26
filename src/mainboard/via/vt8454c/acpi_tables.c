/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local Apic */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *) current, 0, 0);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2, IO_APIC_ADDR, 0);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current, 0, 9, 9, 0x000f);	// low/level

	/* LAPIC_NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *) current, 0, 0x0005, 1);	// high/edge

	return current;
}
