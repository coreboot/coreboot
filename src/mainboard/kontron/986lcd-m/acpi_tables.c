/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2007-2008 coresystems GmbH
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

extern unsigned char AmlCode[];

typedef struct acpi_oemb {
	acpi_header_t header;
	u8  ss;
	u16 iost;
	u32 topm;
	u32 roms;
	u32 mg1b;
	u32 mg1l;
	u32 mg2b;
	u32 mg2l;
	u8  rsvd;
	u8  dmax;
	u32 hpta;
	u32 cpb0;
	u32 cpb1;
	u32 cpb2;
	u32 cpb3;
	u8  assb;
	u8  aotb;
	u32 aaxb;
	u8  smif;
	u8  dtse;
	u8  dts1;
	u8  dts2;
	u8  mpen;
} __attribute__((packed)) acpi_oemb_t;

void acpi_create_oemb(acpi_oemb_t *oemb)
{
	acpi_header_t *header = &(oemb->header);
	unsigned long tolud;

	memset (oemb, 0, sizeof(oemb));

	/* fill out header fields */
	memcpy(header->signature, "OEMB", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, "COREBOOT", 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_oemb_t);
	header->revision = 1;

	oemb->ss   =   0x09; // ss1 + ss 4
	oemb->iost = 0x0403; // ??

        tolud = pci_read_config32(dev_find_slot(0, PCI_DEVFN(2, 0)), 0x5c);
	oemb->topm = tolud;

	oemb->roms = 0xfff00000; // 1M hardcoded

	oemb->mg1b = 0x000d0000;
	oemb->mg1l = 0x00010000;

	oemb->mg2b = tolud;
	oemb->mg2l = 0-tolud;

	oemb->dmax = 0x87;
	oemb->hpta = 0x000e36c0;

	header->checksum =
	    acpi_checksum((void *) oemb, sizeof(acpi_oemb_t));

};

unsigned long acpi_fill_mcfg(unsigned long current)
{
#if 0
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
#endif
	return current;
}

void acpi_create_intel_hpet(acpi_hpet_t * hpet)
{
#define HPET_ADDR  0xfe800000ULL
	acpi_header_t *header = &(hpet->header);
	acpi_addr_t *addr = &(hpet->addr);

	memset((void *) hpet, 0, sizeof(acpi_hpet_t));

	/* fill out header fields */
	memcpy(header->signature, HPET_NAME, 4);
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

	hpet->id = 0x80861234;	/* VIA */
	hpet->number = 0x00;
	hpet->min_tick = 0x0090;

	header->checksum =
	    acpi_checksum((void *) hpet, sizeof(acpi_hpet_t));
}



#define IO_APIC_ADDR	0xfec00000UL

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local Apic */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *) current, 1, 0);
	// This one is for the second core... Will it hurt?
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *) current, 2, 1);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2, IO_APIC_ADDR, 0);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *) current, 0, 9, 9, 0x000d);	// high/level

	return current;
}

unsigned long acpi_fill_srat(unsigned long current)
{
	/* No NUMA, no SRAT */
	return current;
}


#define ALIGN_CURRENT current = ((current + 0x0f) & -0x10)
unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	int i;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_mcfg_t *mcfg;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_oemb_t *oemb;
	acpi_header_t *dsdt;

	/* Align ACPI tables to 16byte */
	start = (start + 0x0f) & -0x10;
	current = start;

	printk_info("ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	ALIGN_CURRENT;
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);
	ALIGN_CURRENT;

	/* clear all table memory */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt);
	acpi_write_rsdt(rsdt);

	/*
	 * We explicitly add these tables later on:
	 */
#if 0
	printk_debug("ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	ALIGN_CURRENT;
	acpi_create_intel_hpet(hpet);
	acpi_add_table(rsdt, hpet);
#endif
	/* If we want to use HPET Timers Linux wants an MADT */
	printk_debug("ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdt, madt);
#if 0
	printk_debug("ACPI:    * MCFG\n");
	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	current += mcfg->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdt, mcfg);
#endif

	printk_debug("ACPI:    * OEMB\n");
	oemb=(acpi_oemb_t *)current;
	current += sizeof(acpi_oemb_t);
	ALIGN_CURRENT;
	acpi_create_oemb(oemb);
	acpi_add_table(rsdt, oemb);

	printk_debug("ACPI:     * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	ALIGN_CURRENT;
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *) current;
	current += ((acpi_header_t *) AmlCode)->length;
	ALIGN_CURRENT;
	memcpy((void *) dsdt, (void *) AmlCode,
	       ((acpi_header_t *) AmlCode)->length);

#if 1
	for (i=0; i < dsdt->length; i++) {
		if (*(u32*)(((u32)dsdt) + i) == 0xC0DEBABE) {
			printk_debug("ACPI: Patching up DSDT at offset 0x%04x -> 0x%08x\n", i, 0x24 + (u32)oemb);
			*(u32*)(((u32)dsdt) + i) = 0x24 + (u32)oemb;
			break;
		}
	}

	/* We patched up the DSDT, so we need to recalculate the checksum */
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum(dsdt, dsdt->length);
#endif

	printk_debug("ACPI:     * DSDT @ %08x Length %x\n", dsdt,
		     dsdt->length);

	printk_debug("ACPI:     * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	ALIGN_CURRENT;

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdt, fadt);
	printk_debug("current = %x\n", current);

	printk_debug("ACPI:     * DMI (Linux workaround)\n");
	memcpy((void *)0xfff80, dmi_table, DMI_TABLE_SIZE);

	printk_info("ACPI: done.\n");
	return current;
}
