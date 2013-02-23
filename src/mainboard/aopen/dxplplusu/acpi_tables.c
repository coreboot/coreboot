/*
 * This file is part of the coreboot project.
 *
 * Written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *  (C) 2005 Digital Design Corporation
 *
 * Ported to Intel XE7501DEVKIT by Agami Aruma
 * Ported to AOpen DXPL Plus-U by Kyösti Mälkki
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <assert.h>
#include "bus.h"

extern unsigned char AmlCode[];

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

unsigned long acpi_fill_slit(unsigned long current)
{
	// Not implemented
	return current;
}

unsigned long acpi_fill_srat(unsigned long current)
{
	// Not implemented
	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int irq_start = 0;
	device_t dev = 0;
	struct resource* res = NULL;

	// SJM: Hard-code CPU LAPIC entries for now
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 0);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 1, 6);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 2, 1);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 3, 7);

	// Southbridge IOAPIC
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_ICH4, 0xfec00000, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;

	// P64H2 Bus B IOAPIC
	dev = dev_find_slot(PCI_BUS_E7501_HI_B, PCI_DEVFN(28, 0));
	if (!dev)
		BUG();		// Config.lb error?
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_P64H2_BUS_B, res->base, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;

	// P64H2 Bus A IOAPIC
	dev = dev_find_slot(PCI_BUS_E7501_HI_B, PCI_DEVFN(30, 0));
	if (!dev)
		BUG();		// Config.lb error?
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_P64H2_BUS_A, res->base, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;


	// Map ISA IRQ 0 to IRQ 2
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 1, 0, 2, 0);

	// IRQ9 differs from ISA standard - ours is active high, level-triggered
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 0, 9, 9, 0xD);

	return current;
}

#define ALIGN_CURRENT current = (ALIGN(current, 16))
unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_madt_t *madt;
	acpi_facs_t *facs;
	acpi_fadt_t *fadt;
	acpi_header_t *dsdt;

	current = start;

	/* Align ACPI tables to 16byte */
	ALIGN_CURRENT;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	ALIGN_CURRENT;
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);
	ALIGN_CURRENT;

	/* clear all table memory */
	memset((void *)start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, NULL);
	acpi_write_rsdt(rsdt);

	/*
	 * We explicitly add these tables later on:
	 */
	printk(BIOS_DEBUG, "ACPI:     * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	ALIGN_CURRENT;

	printk(BIOS_DEBUG, "ACPI:     * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	ALIGN_CURRENT;
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *)current;
	memcpy(dsdt,(void *)AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	ALIGN_CURRENT;
	memcpy(dsdt,(void *)AmlCode, dsdt->length);
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum((void *)dsdt,dsdt->length);
	printk(BIOS_DEBUG, "ACPI:     * DSDT @ %p Length %x\n",dsdt,dsdt->length);

	acpi_create_fadt(fadt,facs,dsdt);
	acpi_add_table(rsdp,fadt);

	printk(BIOS_DEBUG, "ACPI:    * MADT\n");
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current+=madt->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdp,madt);

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}

