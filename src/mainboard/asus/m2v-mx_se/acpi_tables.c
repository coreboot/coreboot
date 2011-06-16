/*
 * This file is part of the coreboot project.
 *
 * Written by Stefan Reinauer <stepan@openbios.org>.
 * ACPI FADT, FACS, and DSDT table support added by
 *
 * Copyright (C) 2004 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include "southbridge/via/vt8237r/vt8237r.h"
#include "southbridge/via/k8t890/k8t890.h"
#include "northbridge/amd/amdk8/acpi.h"
#include <cpu/amd/model_fxx_powernow.h>

extern const unsigned char AmlCode[];

unsigned long acpi_fill_mcfg(unsigned long current)
{
	device_t dev;
	struct resource *res;

	dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_K8M890CE_5, 0);
	if (!dev)
		return current;

	res = find_resource(dev, K8T890_MMCONFIG_MBAR);
	if (res) {
		current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)
				current, res->base, 0x0, 0x0, 0xff);
	}
	return current;
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

unsigned long acpi_fill_ssdt_generator(unsigned long current, const char *oem_table_id)
{
	k8acpi_write_vars();
	amd_model_fxx_generate_powernow(0, 0, 0);
	acpigen_write_mainboard_resources("\\_SB.PCI0.MBRS", "_CRS");
	return (unsigned long) (acpigen_get_current());
}

unsigned long write_acpi_tables(unsigned long start)
{
	return acpi_write_tables(start, AmlCode);
}
