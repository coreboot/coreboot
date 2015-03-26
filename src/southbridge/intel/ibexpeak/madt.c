/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
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
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
					   1, IO_APIC_ADDR, 0);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2,
						MP_IRQ_POLARITY_DEFAULT |
						MP_IRQ_TRIGGER_DEFAULT);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9,
						MP_IRQ_POLARITY_HIGH |
						MP_IRQ_TRIGGER_LEVEL);

	/* LAPIC_NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
					      current, 0,
					      MP_IRQ_POLARITY_HIGH |
					      MP_IRQ_TRIGGER_EDGE, 0x01);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
					      current, 1, MP_IRQ_POLARITY_HIGH |
					      MP_IRQ_TRIGGER_EDGE, 0x01);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
					      current, 2, MP_IRQ_POLARITY_HIGH |
					      MP_IRQ_TRIGGER_EDGE, 0x01);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
					      current, 3, MP_IRQ_POLARITY_HIGH |
					      MP_IRQ_TRIGGER_EDGE, 0x01);
	return current;
}
