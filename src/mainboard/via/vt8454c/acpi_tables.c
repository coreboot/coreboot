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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "dmi.h"

extern const unsigned char AmlCode[];

unsigned long acpi_fill_mcfg(unsigned long current)
{
	device_t dev;
	u64 mmcfg;

	dev = dev_find_device(0x1106, 0x324b, 0);	// 0:0x13.0
	if (!dev)
		return current;

	// MMCFG not supported or not enabled.
	if ((pci_read_config8(dev, 0x40) & 0xC0) != 0xC0)
		return current;

	mmcfg = ((u64) pci_read_config8(dev, 0x41)) << 28;
	if (!mmcfg)
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current, mmcfg, 0x0, 0x0, 0xff);

	return current;
}


static void acpi_create_via_hpet(acpi_hpet_t * hpet)
{
#define HPET_ADDR  0xfe800000ULL
	acpi_header_t *header = &(hpet->header);
	acpi_addr_t *addr = &(hpet->addr);

	memset((void *) hpet, 0, sizeof(acpi_hpet_t));

	/* fill out header fields */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, "COREBOOT", 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_hpet_t);
	header->revision = 1;

	/* fill out HPET address */
	// XXX factory bios just puts an address here -- who's right?
	addr->space_id = 0;	/* Memory */
	addr->bit_width = 64;
	addr->bit_offset = 0;
	addr->addrl = HPET_ADDR & 0xffffffff;
	addr->addrh = HPET_ADDR >> 32;

	hpet->id = 0x11068201;	/* VIA */
	hpet->number = 0x00;
	hpet->min_tick = 0x0090;

	header->checksum =
	    acpi_checksum((void *) hpet, sizeof(acpi_hpet_t));
}



#define IO_APIC_ADDR	0xfec00000UL

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

unsigned long acpi_fill_slit(unsigned long current)
{
	// Not implemented
	return current;
}

unsigned long acpi_fill_srat(unsigned long current)
{
	/* No NUMA, no SRAT */
	return current;
}

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_mcfg_t *mcfg;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *dsdt;

	/* Align ACPI tables to 16byte */
	start = (start + 0x0f) & -0x10;
	current = start;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* clear all table memory */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, NULL);
	acpi_write_rsdt(rsdt);

	/*
	 * We explicitly add these tables later on:
	 */

	printk(BIOS_DEBUG, "ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	acpi_create_via_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	/* If we want to use HPET Timers Linux wants an MADT */
	printk(BIOS_DEBUG, "ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	acpi_add_table(rsdp, madt);

	printk(BIOS_DEBUG, "ACPI:    * MCFG\n");
	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	current += mcfg->header.length;
	acpi_add_table(rsdp, mcfg);

	printk(BIOS_DEBUG, "ACPI:     * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *) current;
	dsdt = (acpi_header_t *)current;
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	memcpy(dsdt, &AmlCode, dsdt->length);
#ifdef DONT_TRUST_IASL
	dsdt->checksum = 0;	// don't trust intel iasl compiler to get this right
	dsdt->checksum = acpi_checksum(dsdt, dsdt->length);
#endif
	printk(BIOS_DEBUG, "ACPI:     * DSDT @ %p Length %x\n", dsdt,
		     dsdt->length);
	printk(BIOS_DEBUG, "ACPI:     * FADT\n");

	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

	printk(BIOS_DEBUG, "ACPI:     * DMI (Linux workaround)\n");
	memcpy((void *)0xfff80, dmi_table, DMI_TABLE_SIZE);

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}
