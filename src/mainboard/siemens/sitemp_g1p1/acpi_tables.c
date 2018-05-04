/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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

#include <compiler.h>
#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/amdk8_sysconf.h>
#include <../../../northbridge/amd/amdk8/acpi.h>
#include <arch/cpu.h>
#include <cpu/amd/powernow.h>
#include <southbridge/amd/rs690/rs690.h>
#include "mainboard.h"
#include <cbmem.h>

#define GLOBAL_VARS_SIZE 0x100

typedef struct {
	/* Miscellaneous */
	u16 osys;
	u16 linx;
	u32	pcba;
	u8  mpen;
	u8 reserv[247];
} __packed global_vars_t;

static void acpi_write_gvars(global_vars_t *gvars)
{
	struct device *dev;
	struct resource *res;

	memset((void *)gvars, 0, GLOBAL_VARS_SIZE);

	gvars->pcba = EXT_CONF_BASE_ADDRESS;
	dev = dev_find_slot(0, PCI_DEVFN(0,0));
	res = probe_resource(dev, 0x1C);
	if( res )
		gvars->pcba = res->base;

	gvars->mpen = 1;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write SB600 IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2,
					   IO_APIC_ADDR, 0);
#if !IS_ENABLED(CONFIG_LINT01_CONVERSION)
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0);

	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);
#else
						/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */

	/* create all subtables for processors */
	current = acpi_create_madt_lapic_nmis(current, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);
	/* 1: LINT1 connect to NMI */
	set_nbcfg_enable_bits(dev_find_slot(0, PCI_DEVFN(0x18, 0)), 0x68, 1 << 16, 1 << 16); // Local Interrupt Conversion Enable
#endif
	return current;
}

void mainboard_inject_dsdt(struct device *device)
{
	global_vars_t *gnvs = cbmem_add (CBMEM_ID_ACPI_GNVS, GLOBAL_VARS_SIZE);

	if (gnvs) {
		memset(gnvs, 0, sizeof(*gnvs));
		acpi_write_gvars(gnvs);

		/* Add it to SSDT.  */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (u32) gnvs);
		acpigen_pop_len();
	}
}
