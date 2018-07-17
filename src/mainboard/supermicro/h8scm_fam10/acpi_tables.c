/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/amdfam10_sysconf.h>

#include "mb_sysconf.h"

unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *dev;
	u32 dword;
	u32 gsi_base = 0;
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write SB700 IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2,
					   IO_APIC_ADDR, gsi_base);
	/* IOAPIC on rs5690 */
	gsi_base += 24;		/* SB700 has 24 IOAPIC entries. */
	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (dev) {
		pci_write_config32(dev, 0xF8, 0x1);
		dword = pci_read_config32(dev, 0xFC) & 0xfffffff0;
		current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2+1,
						   dword, gsi_base);
	}


	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9, 0xF);
	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */

	/* create all subtables for processors */
	/* current = acpi_create_madt_lapic_nmis(current, 5, 1); */
	/* 1: LINT1 connect to NMI */

	return current;
}

unsigned long acpi_fill_ivrs_ioapic(acpi_ivrs_t *ivrs, unsigned long current)
{
	uint8_t *p;

	uint32_t apicid_sp5100;
	uint32_t apicid_sr5650;

	apicid_sp5100 = 0x20;
	apicid_sr5650 = apicid_sp5100 + 1;

	/* Describe NB IOAPIC */
	p = (uint8_t *)current;
	p[0] = 0x48;                    /* Entry type */
	p[1] = 0;                       /* Device */
	p[2] = 0;                       /* Bus */
	p[3] = 0x0;                     /* Data */
	p[4] = apicid_sr5650;           /* IOAPIC ID */
	p[5] = 0x1;                     /* Device 0 Function 1 */
	p[6] = 0x0;                     /* Northbridge bus */
	p[7] = 0x1;                     /* Variety */
	current += 8;

	/* Describe SB IOAPIC */
	p = (uint8_t *)current;
	p[0] = 0x48;                    /* Entry type */
	p[1] = 0;                       /* Device */
	p[2] = 0;                       /* Bus */
	p[3] = 0xd7;                    /* Data */
	p[4] = apicid_sp5100;           /* IOAPIC ID */
	p[5] = 0x14 << 3;               /* Device 0x14 Function 0 */
	p[6] = 0x0;                     /* Southbridge bus */
	p[7] = 0x1;                     /* Variety */
	current += 8;

	return current;
}
