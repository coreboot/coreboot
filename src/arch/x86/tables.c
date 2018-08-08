/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2005 Steve Magnani
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include <cpu/cpu.h>
#include <bootmem.h>
#include <bootstate.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <arch/pirq_routing.h>
#include <arch/smp/mpspec.h>
#include <arch/acpi.h>
#include <string.h>
#include <cbmem.h>
#include <smbios.h>

static unsigned long write_pirq_table(unsigned long rom_table_end)
{
	unsigned long high_table_pointer;

#define MAX_PIRQ_TABLE_SIZE (4 * 1024)
	post_code(0x9a);

	/* This table must be between 0x0f0000 and 0x100000 */
	rom_table_end = write_pirq_routing_table(rom_table_end);
	rom_table_end = ALIGN(rom_table_end, 1024);

	/* And add a high table version for those payloads that
	 * want to live in the F segment
	 */
	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_PIRQ,
		MAX_PIRQ_TABLE_SIZE);
	if (high_table_pointer) {
		unsigned long new_high_table_pointer;
		new_high_table_pointer =
			write_pirq_routing_table(high_table_pointer);
		// FIXME make pirq table code intelligent enough to know how
		// much space it's going to need.
		if (new_high_table_pointer > (high_table_pointer
			+ MAX_PIRQ_TABLE_SIZE))
			printk(BIOS_ERR, "ERROR: Increase PIRQ size.\n");
		printk(BIOS_DEBUG, "PIRQ table: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	}

	return rom_table_end;
}

static unsigned long write_mptable(unsigned long rom_table_end)
{
	unsigned long high_table_pointer;

#define MAX_MP_TABLE_SIZE (4 * 1024)
	post_code(0x9b);

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	rom_table_end = write_smp_table(rom_table_end);
	rom_table_end = ALIGN(rom_table_end, 1024);

	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_MPTABLE,
		MAX_MP_TABLE_SIZE);
	if (high_table_pointer) {
		unsigned long new_high_table_pointer;
		new_high_table_pointer = write_smp_table(high_table_pointer);
		// FIXME make mp table code intelligent enough to know how
		// much space it's going to need.
		if (new_high_table_pointer > (high_table_pointer
			+ MAX_MP_TABLE_SIZE))
			printk(BIOS_ERR, "ERROR: Increase MP table size.\n");

		printk(BIOS_DEBUG, "MP table: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	}

	return rom_table_end;
}

static unsigned long write_acpi_table(unsigned long rom_table_end)
{
	unsigned long high_table_pointer;

#define MAX_ACPI_SIZE (144 * 1024)

	post_code(0x9c);

	/* Write ACPI tables to F segment and high tables area */

	/* Ok, this is a bit hacky still, because some day we want to have this
	 * completely dynamic. But right now we are setting fixed sizes.
	 * It's probably still better than the old high_table_base code because
	 * now at least we know when we have an overflow in the area.
	 *
	 * We want to use 1MB - 64K for Resume backup. We use 512B for TOC and
	 * 512 byte for GDT, 4K for PIRQ and 4K for MP table and 8KB for the
	 * coreboot table. This leaves us with 47KB for all of ACPI. Let's see
	 * how far we get.
	 */
	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_ACPI,
		MAX_ACPI_SIZE);
	if (high_table_pointer) {
		unsigned long acpi_start = high_table_pointer;
		unsigned long new_high_table_pointer;

		rom_table_end = ALIGN(rom_table_end, 16);
		new_high_table_pointer = write_acpi_tables(high_table_pointer);
		if (new_high_table_pointer > (high_table_pointer
			+ MAX_ACPI_SIZE))
			printk(BIOS_ERR, "ERROR: Increase ACPI size\n");
		printk(BIOS_DEBUG, "ACPI tables: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);

		/* Now we need to create a low table copy of the RSDP. */

		/* First we look for the high table RSDP */
		while (acpi_start < new_high_table_pointer) {
			if (memcmp(((acpi_rsdp_t *)acpi_start)->signature,
				RSDP_SIG, 8) == 0)
				break;
			acpi_start++;
		}

		/* Now, if we found the RSDP, we take the RSDT and XSDT pointer
		 * from it in order to write the low RSDP
		 */
		if (acpi_start < new_high_table_pointer) {
			acpi_rsdp_t *low_rsdp = (acpi_rsdp_t *)rom_table_end,
				    *high_rsdp = (acpi_rsdp_t *)acpi_start;

			/* Technically rsdp length varies but coreboot always
			   writes longest size available.  */
			memcpy(low_rsdp, high_rsdp, sizeof(acpi_rsdp_t));
		} else {
			printk(BIOS_ERR,
				"ERROR: Didn't find RSDP in high table.\n");
		}
		rom_table_end = ALIGN(rom_table_end + sizeof(acpi_rsdp_t), 16);
	} else {
		rom_table_end = write_acpi_tables(rom_table_end);
		rom_table_end = ALIGN(rom_table_end, 1024);
	}

	return rom_table_end;
}

static unsigned long write_smbios_table(unsigned long rom_table_end)
{
	unsigned long high_table_pointer;

#define MAX_SMBIOS_SIZE 2048

	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_SMBIOS,
		MAX_SMBIOS_SIZE);
	if (high_table_pointer) {
		unsigned long new_high_table_pointer;

		new_high_table_pointer =
			smbios_write_tables(high_table_pointer);
		rom_table_end = ALIGN(rom_table_end, 16);
		memcpy((void *)rom_table_end, (void *)high_table_pointer,
			sizeof(struct smbios_entry));
		rom_table_end += sizeof(struct smbios_entry);

		if (new_high_table_pointer > (high_table_pointer
			+ MAX_SMBIOS_SIZE))
			printk(BIOS_ERR, "ERROR: Increase SMBIOS size\n");
		printk(BIOS_DEBUG, "SMBIOS tables: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	} else {
		unsigned long new_rom_table_end;

		new_rom_table_end = smbios_write_tables(rom_table_end);
		printk(BIOS_DEBUG, "SMBIOS size %ld bytes\n", new_rom_table_end
			- rom_table_end);
		rom_table_end = ALIGN(new_rom_table_end, 16);
	}

	return rom_table_end;
}

/* Start forwarding table at 0x500, so we don't run into conflicts with the BDA
 * in case our data structures grow beyond 0x400. Only GDT
 * and the coreboot table use low_tables.
 */
#define FORWARDING_TABLE_ADDR ((uintptr_t)0x500)
static uintptr_t forwarding_table = FORWARDING_TABLE_ADDR;

/*
 * For EARLY_EBDA_INIT the BDA area will be wiped on the resume path which
 * has the forwarding table entry. Therefore, when tables are written an
 * entry is placed in cbmem that can be restored on OS resume to the proper
 * location.
 */
static void stash_forwarding_table(uintptr_t addr, size_t sz)
{
	void *cbmem_addr = cbmem_add(CBMEM_ID_CBTABLE_FWD, sz);

	if (cbmem_addr == NULL) {
		printk(BIOS_ERR, "Unable to allocate CBMEM forwarding entry.\n");
		return;
	}

	memcpy(cbmem_addr, (void *)addr, sz);
}

static void restore_forwarding_table(void *dest)
{
	const struct cbmem_entry *fwd_entry;

	fwd_entry = cbmem_entry_find(CBMEM_ID_CBTABLE_FWD);

	if (fwd_entry == NULL) {
		printk(BIOS_ERR, "Unable to restore CBMEM forwarding entry.\n");
		return;
	}

	memcpy(dest, cbmem_entry_start(fwd_entry), cbmem_entry_size(fwd_entry));
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY,
	restore_forwarding_table, (void *)FORWARDING_TABLE_ADDR);

void arch_write_tables(uintptr_t coreboot_table)
{
	size_t sz;
	unsigned long rom_table_end = 0xf0000;

	/* This table must be between 0x0f0000 and 0x100000 */
	if (IS_ENABLED(CONFIG_GENERATE_PIRQ_TABLE))
		rom_table_end = write_pirq_table(rom_table_end);

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	if (IS_ENABLED(CONFIG_GENERATE_MP_TABLE))
		rom_table_end = write_mptable(rom_table_end);

	if (IS_ENABLED(CONFIG_HAVE_ACPI_TABLES))
		rom_table_end = write_acpi_table(rom_table_end);

	if (IS_ENABLED(CONFIG_GENERATE_SMBIOS_TABLES))
		rom_table_end = write_smbios_table(rom_table_end);

	sz = write_coreboot_forwarding_table(forwarding_table, coreboot_table);

	stash_forwarding_table(forwarding_table, sz);

	forwarding_table += sz;
	/* Align up to page boundary for historical consistency. */
	forwarding_table = ALIGN_UP(forwarding_table, 4*KiB);

	/* Tell static analysis we know value is left unused. */
	(void)rom_table_end;
}

void bootmem_arch_add_ranges(void)
{
	/* Memory from 0 through the forwarding_table is reserved. */
	const uintptr_t base = 0;

	bootmem_add_range(base, forwarding_table - base, BM_MEM_TABLE);
}
