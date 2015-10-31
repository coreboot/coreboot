/*
 * This file is part of the coreboot project.
 *
 * Written by Stefan Reinauer <stepan@openbios.org>.
 * ACPI FADT, FACS, and DSDT table support added by
 *
 * Copyright (C) 2004 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include "southbridge/via/vt8237r/vt8237r.h"
#include "southbridge/via/k8t890/k8t890.h"
#include "northbridge/amd/amdk8/acpi.h"
#include <cpu/amd/powernow.h>
#include <cpu/amd/amdk8_sysconf.h>

void get_bus_conf(void)
{
	/* FIXME: implement this.  */
}

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base = 0x18;

	/* Create all subtables for processors. */
	current = acpi_create_madt_lapics(current);

	/* Write SB IOAPIC. */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				VT8237R_APIC_ID, IO_APIC_ADDR, 0);

	/* Write NB IOAPIC. */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				K8T890_APIC_ID, K8T890_APIC_BASE, gsi_base);

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
