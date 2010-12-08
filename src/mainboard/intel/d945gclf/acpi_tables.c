/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <arch/ioapic.h>
#include "dmi.h"

#define OLD_ACPI 0

extern const unsigned char AmlCode[];
#if CONFIG_HAVE_ACPI_SLIC
unsigned long acpi_create_slic(unsigned long current);
#endif

#if OLD_ACPI
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
#endif

#include "southbridge/intel/i82801gx/nvs.h"

#if OLD_ACPI
static void acpi_create_oemb(acpi_oemb_t *oemb)
{
	acpi_header_t *header = &(oemb->header);
	unsigned long tolud;

	memset (oemb, 0, sizeof(*oemb));

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
#endif

static void acpi_create_gnvs(global_nvs_t *gnvs)
{
	memset((void *)gnvs, 0, sizeof(*gnvs));
	gnvs->apic = 1;
	gnvs->mpen = 1; /* Enable Multi Processing */
}

static void acpi_create_intel_hpet(acpi_hpet_t * hpet)
{
#define HPET_ADDR  0xfed00000ULL
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
	addr->space_id = 0;	/* Memory */
	addr->bit_width = 64;
	addr->bit_offset = 0;
	addr->addrl = HPET_ADDR & 0xffffffff;
	addr->addrh = HPET_ADDR >> 32;

	hpet->id = 0x8086a201;	/* Intel */
	hpet->number = 0x00;
	hpet->min_tick = 0x0080;

	header->checksum =
	    acpi_checksum((void *) hpet, sizeof(acpi_hpet_t));
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_HIGH);

	return current;
}

unsigned long acpi_fill_ssdt_generator(unsigned long current, const char *oem_table_id)
{
	generate_cpu_entries();
	return (unsigned long) (acpigen_get_current());
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

#if CONFIG_HAVE_SMI_HANDLER
void smm_setup_structures(void *gnvs, void *tcg, void *smi1);
#endif

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
#if CONFIG_HAVE_ACPI_SLIC
	acpi_header_t *slic;
#endif
#if OLD_ACPI
	acpi_oemb_t *oemb;
#endif
	acpi_header_t *ssdt;
	acpi_header_t *dsdt;

	current = start;

	/* Align ACPI tables to 16byte */
	ALIGN_CURRENT;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	ALIGN_CURRENT;
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);
	ALIGN_CURRENT;

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
	ALIGN_CURRENT;
	acpi_create_intel_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	/* If we want to use HPET Timers Linux wants an MADT */
	printk(BIOS_DEBUG, "ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdp, madt);

	printk(BIOS_DEBUG, "ACPI:    * MCFG\n");
	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	current += mcfg->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdp, mcfg);

#if OLD_ACPI
	printk(BIOS_DEBUG, "ACPI:    * OEMB\n");
	oemb=(acpi_oemb_t *)current;
	current += sizeof(acpi_oemb_t);
	ALIGN_CURRENT;
	acpi_create_oemb(oemb);
	acpi_add_table(rsdp, oemb);
#endif

	printk(BIOS_DEBUG, "ACPI:     * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	ALIGN_CURRENT;
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *) current;
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	memcpy(dsdt, &AmlCode, dsdt->length);

#if OLD_ACPI
	for (i=0; i < dsdt->length; i++) {
		if (*(u32*)(((u32)dsdt) + i) == 0xC0DEBEEF) {
			printk(BIOS_DEBUG, "ACPI: Patching up DSDT at offset 0x%04x -> 0x%08x\n", i, 0x24 + (u32)oemb);
			*(u32*)(((u32)dsdt) + i) = 0x24 + (u32)oemb;
			break;
		}
	}
#endif

	ALIGN_CURRENT;

	/* Pack GNVS into the ACPI table area */
	for (i=0; i < dsdt->length; i++) {
		if (*(u32*)(((u32)dsdt) + i) == 0xC0DEBABE) {
			printk(BIOS_DEBUG, "ACPI: Patching up global NVS in DSDT at offset 0x%04x -> 0x%08lx\n", i, current);
			*(u32*)(((u32)dsdt) + i) = current; // 0x92 bytes
			break;
		}
	}

	/* And fill it */
	acpi_create_gnvs((global_nvs_t *)current);

	current += 0x100;
	ALIGN_CURRENT;

#if CONFIG_HAVE_SMI_HANDLER
	/* And tell SMI about it */
	smm_setup_structures((void *)current, NULL, NULL);
#endif

	/* We patched up the DSDT, so we need to recalculate the checksum */
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum((void *)dsdt, dsdt->length);

	printk(BIOS_DEBUG, "ACPI:     * DSDT @ %p Length %x\n", dsdt,
		     dsdt->length);

#if CONFIG_HAVE_ACPI_SLIC
	printk(BIOS_DEBUG, "ACPI:     * SLIC\n");
	slic = (acpi_header_t *)current;
	current += acpi_create_slic(current);
	ALIGN_CURRENT;
	acpi_add_table(rsdp, slic);
#endif

	printk(BIOS_DEBUG, "ACPI:     * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	ALIGN_CURRENT;

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

	printk(BIOS_DEBUG, "ACPI:     * SSDT\n");
	ssdt = (acpi_header_t *)current;
	acpi_create_ssdt_generator(ssdt, "DYNADATA");
	current += ssdt->length;
	acpi_add_table(rsdp, ssdt);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	printk(BIOS_DEBUG, "ACPI:     * DMI (Linux workaround)\n");
	memcpy((void *)0xfff80, dmi_table, DMI_TABLE_SIZE);
#if CONFIG_WRITE_HIGH_TABLES == 1
	memcpy((void *)current, dmi_table, DMI_TABLE_SIZE);
	current += DMI_TABLE_SIZE;
	ALIGN_CURRENT;
#endif

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}
