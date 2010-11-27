/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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
#include <device/device.h>
#include <device/pci_ids.h>

extern const unsigned char AmlCode[];

unsigned long __attribute__((weak)) acpi_fill_slit(unsigned long current)
{
	// Not implemented
	return current;
}

unsigned long __attribute__((weak)) acpi_fill_srat(unsigned long current)
{
	// Not implemented
	return current;
}

unsigned long __attribute__((weak)) acpi_fill_madt(unsigned long current)
{
	/* mainboard has no ioapic */
	return current;
}

unsigned long __attribute__((weak)) acpi_fill_mcfg(unsigned long current)
{
	/* chipset doesn't have mmconfig */
	return current;
}

unsigned long __attribute__((weak)) acpi_fill_ssdt_generator(unsigned long current,
						 const char *oem_table_id)
{
	acpigen_write_mainboard_resources("\\_SB.PCI0.MBRS", "_CRS");
	return (unsigned long) acpigen_get_current();
}

unsigned long __attribute__((weak)) write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_madt_t *madt;
	acpi_header_t *ssdt;
	acpi_header_t *dsdt;

	/* Align ACPI tables to 16 byte. */
	start = (start + 0x0f) & -0x10;
	current = start;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT table. */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* Clear all table memory. */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, NULL);
	acpi_write_rsdt(rsdt);

	/* We explicitly add these tables later on: */
	printk(BIOS_DEBUG, "ACPI:     * FACS\n");

	/* we should align FACS to 64B as per ACPI specs */
	current = ALIGN(current, 64);
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *)current;
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	memcpy(dsdt, &AmlCode, dsdt->length);
	/* Don't trust iasl to get checksum right. */
	dsdt->checksum = 0; /* needs to be set to 0 first (part of csum) */
	dsdt->checksum = acpi_checksum((u8*)dsdt, dsdt->length);
	printk(BIOS_DEBUG, "ACPI:     * DSDT @ %p Length %x\n", dsdt,
		     dsdt->length);
	printk(BIOS_DEBUG, "ACPI:     * FADT\n");

	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

	printk(BIOS_DEBUG, "ACPI:    * MADT\n");
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	if (madt->header.length > sizeof(acpi_madt_t)) {
		current += madt->header.length;
		acpi_add_table(rsdp, madt);
	} else {
		/* don't add empty madt */
		current = (unsigned long)madt;
	}

	printk(BIOS_DEBUG, "ACPI:    * SSDT\n");
	ssdt = (acpi_header_t *)current;
	acpi_create_ssdt_generator(ssdt, "DYNADATA");
	current += ssdt->length;
	acpi_add_table(rsdp, ssdt);

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}
