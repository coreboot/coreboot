/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include <arch/ioapic.h>
#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdfam12.h>

extern u32 apicid_sb900;

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 0);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 1, 1);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 2, 2);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 3, 3);

	/* Write SB900 IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, apicid_sb900,
		   IO_APIC_ADDR, 0);

	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		  current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		  current, 0, 9, 9, 0xF);

	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */

	/* create all subtables for processors */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 0, 5, 1);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 1, 5, 1);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 2, 5, 1);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 3, 5, 1);
	/* 1: LINT1 connect to NMI */

	return current;
}
