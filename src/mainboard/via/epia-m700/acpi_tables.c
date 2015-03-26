/*
 * This file is part of the coreboot project.
 *
 * LinuxBIOS ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 * ACPI FADT, FACS, and DSDT table support added by
 * Nick Barker <nick.barker9@btinternet.com>, and those portions
 * (C) Copyright 2004 Nick Barker
 * (C) Copyright 2005 Stefan Reinauer
 * (C) Copyright 2009 One Laptop per Child, Association, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/*
 * Most parts of this file copied from asus\a8v-e_se\acpi_tables.c,
 * acpi_is_wakeup() is from Rudolf's S3 patch and SSDT was added.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include "northbridge/via/vx800/vx800.h"

extern u32 wake_vec;

/*
 * These four macros are copied from <arch/smp/mpspec.h>, I have to do this
 * since the "CONFIG_GENERATE_MP_TABLE = 0", and also since
 * mainboard/via/... have no mptable.c (so that I can not set
 * "CONFIG_GENERATE_MP_TABLE = 1" as many other mainboards.
 * So I have to copy these four to here. acpi_fill_madt() needs this.
 */
#define MP_IRQ_POLARITY_HIGH	0x1
#define MP_IRQ_POLARITY_LOW	0x3
#define MP_IRQ_TRIGGER_EDGE	0x4
#define MP_IRQ_TRIGGER_LEVEL	0xc

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* NO MCFG in VX855, no PCI-E. */
	return current;
}

unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags,
					  u8 lint)
{
	device_t cpu;
	int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
					      current, cpu_index, flags, lint);
		cpu_index++;
	}
	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Create all subtables for processors. */
	current = acpi_create_madt_lapics(current);

	/* Write SB IOAPIC. */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				   VX800SB_APIC_ID, VX800SB_APIC_BASE, 0);

	/* IRQ0 -> APIC IRQ2. */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0x0);

	/* IRQ9 ACPI active low. */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	/* Create all subtables for processors. */
	current = acpi_create_madt_lapic_nmis(current,
			      MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);

	return current;
}
