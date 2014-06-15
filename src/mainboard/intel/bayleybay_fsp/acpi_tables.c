/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <baytrail/acpi.h>
#include <baytrail/nvs.h>
#include <baytrail/iomap.h>


extern const unsigned char AmlCode[];

static void acpi_create_gnvs(global_nvs_t *gnvs)
{
	acpi_init_gnvs(gnvs);

	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* TPM Present */
	gnvs->tpmp = 0;

	/* Enable DPTF */
	gnvs->dpte = 0;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	current = acpi_madt_irq_overrides(current);

	return current;
}

unsigned long acpi_fill_ssdt_generator(unsigned long current,
					const char *oem_table_id)
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

#define ALIGN_CURRENT current = (ALIGN(current, 16))
unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	int i;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_xsdt_t *xsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_mcfg_t *mcfg;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *ssdt;
	acpi_header_t *ssdt2;
	acpi_header_t *dsdt;
	global_nvs_t *gnvs;

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
	xsdt = (acpi_xsdt_t *) current;
	current += sizeof(acpi_xsdt_t);
	ALIGN_CURRENT;

	/* clear all table memory */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, xsdt);
	acpi_write_rsdt(rsdt);
	acpi_write_xsdt(xsdt);

	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	ALIGN_CURRENT;
	acpi_create_facs(facs);
	printk(BIOS_DEBUG, "ACPI:    * FACS @ %p Length %x", facs,
	       facs->length);

	dsdt = (acpi_header_t *) current;
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	ALIGN_CURRENT;
	memcpy(dsdt, &AmlCode, dsdt->length);
	printk(BIOS_DEBUG, "ACPI:    * DSDT @ %p Length %x", dsdt,
	       dsdt->length);

	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	ALIGN_CURRENT;
	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);
	printk(BIOS_DEBUG, "ACPI:    * FADT @ %p Length %x", fadt,
	       fadt->header.length);

	/*
	 * We explicitly add these tables later on:
	 */
	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	ALIGN_CURRENT;
	acpi_create_intel_hpet(hpet);
	acpi_add_table(rsdp, hpet);
	printk(BIOS_DEBUG, "ACPI:    * HPET @ %p Length %x\n", hpet,
	       hpet->header.length);

	/* If we want to use HPET Timers Linux wants an MADT */
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdp, madt);
	printk(BIOS_DEBUG, "ACPI:    * MADT @ %p Length %x\n",madt,
	       madt->header.length);

	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	current += mcfg->header.length;
	ALIGN_CURRENT;
	acpi_add_table(rsdp, mcfg);
	printk(BIOS_DEBUG, "ACPI:    * MCFG @ %p Length %x\n",mcfg,
	       mcfg->header.length);

	/* Update GNVS pointer into CBMEM */
	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		printk(BIOS_DEBUG, "ACPI: Could not find CBMEM GNVS\n");
		gnvs = (global_nvs_t *)current;
	}

	for (i=0; i < dsdt->length; i++) {
		if (*(u32*)(((u32)dsdt) + i) == 0xC0DEBABE) {
			printk(BIOS_DEBUG, "ACPI: Patching up global NVS in "
			       "DSDT at offset 0x%04x -> %p\n", i, gnvs);
			*(u32*)(((u32)dsdt) + i) = (unsigned long)gnvs;
			acpi_save_gnvs((unsigned long)gnvs);
			break;
		}
	}

	/* And fill it */
	acpi_create_gnvs(gnvs);

	/* And tell SMI about it */
#if CONFIG_HAVE_SMI_HANDLER
	smm_setup_structures(gnvs, NULL, NULL);
#endif

	current += sizeof(global_nvs_t);
	ALIGN_CURRENT;

	/* We patched up the DSDT, so we need to recalculate the checksum */
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum((void *)dsdt, dsdt->length);

	printk(BIOS_DEBUG, "ACPI Updated DSDT @ %p Length %x\n", dsdt,
		     dsdt->length);

	ssdt = (acpi_header_t *)current;
	memset(ssdt, 0, sizeof(acpi_header_t));
	acpi_create_ssdt_generator(ssdt, ACPI_TABLE_CREATOR);
	if (ssdt->length) {
		current += ssdt->length;
		acpi_add_table(rsdp, ssdt);
		printk(BIOS_DEBUG, "ACPI:     * SSDT @ %p Length %x\n",ssdt,
		       ssdt->length);
		ALIGN_CURRENT;
	} else {
		ssdt = NULL;
		printk(BIOS_DEBUG, "ACPI:     * SSDT not generated.\n");
	}

	ssdt2 = (acpi_header_t *)current;
	memset(ssdt2, 0, sizeof(acpi_header_t));
	acpi_create_serialio_ssdt(ssdt2);
	if (ssdt2->length) {
		current += ssdt2->length;
		acpi_add_table(rsdp, ssdt2);
		printk(BIOS_DEBUG, "ACPI:     * SSDT2 @ %p Length %x\n",ssdt2,
		       ssdt2->length);
		ALIGN_CURRENT;
	} else {
		ssdt2 = NULL;
		printk(BIOS_DEBUG, "ACPI:     * SSDT2 not generated.\n");
	}

	printk(BIOS_DEBUG, "current = %lx\n", current);

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}
