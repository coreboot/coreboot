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
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/ioapic.h>
#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/mtrr.h>

#include <northbridge/amd/agesa/agesawrapper.h>
#include <cpu/amd/amdfam15.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	device_t dev;
	u32 dword;
	u32 gsi_base = 0;
	u32 apicid_sb700;
	u32 apicid_rd890;

	/*
	 * AGESA v5 Apply apic enumeration rules
	 * For systems with >= 16 APICs, put the IO-APICs at 0..n and
	 * put the local-APICs at m..z
	 * For systems with < 16 APICs, put the Local-APICs at 0..n and
	 * put the IO-APICs at (n + 1)..z
	 */
	if (CONFIG_MAX_CPUS >= 16)
		apicid_sb700 = 0x0;
	else
		apicid_sb700 = CONFIG_MAX_CPUS + 1;
	apicid_rd890 = apicid_sb700 + 1;

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write sb700 IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
			apicid_sb700,
			IO_APIC_ADDR,
			0
			);

	/* IOAPIC on rs5690 */
	gsi_base += IO_APIC_INTERRUPTS;  /* sb700 has 24 IOAPIC entries. */
	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (dev) {
		pci_write_config32(dev, 0xF8, 0x1);
		dword = pci_read_config32(dev, 0xFC) & 0xfffffff0;
		current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				apicid_rd890,
				dword,
				gsi_base
				);
	}

	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current,
			0, //BUS
			0, //SOURCE
			2, //gsirq
			0  //flags
			);

	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */

	/* create all subtables for processors */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 0, 5, 1);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 1, 5, 1);
	/* 1: LINT1 connect to NMI */

	return current;
}
