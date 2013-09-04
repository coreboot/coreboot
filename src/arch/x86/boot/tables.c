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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <arch/pirq_routing.h>
#include <arch/smp/mpspec.h>
#include <arch/acpi.h>
#include <string.h>
#include <cpu/x86/multiboot.h>
#include <cbmem.h>
#include <lib.h>
#include <smbios.h>


void cbmem_arch_init(void)
{
	/* defined in gdt.c */
	move_gdt();
}

struct lb_memory *write_tables(void)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;

	/* Even if high tables are configured, some tables are copied both to
	 * the low and the high area, so payloads and OSes don't need to know
	 * about the high tables.
	 */
	unsigned long high_table_pointer;

#if !CONFIG_DYNAMIC_CBMEM
	cbmem_base_check();
#endif

	rom_table_start = 0xf0000;
	rom_table_end =   0xf0000;

	/* Start low addr at 0x500, so we don't run into conflicts with the BDA
	 * in case our data structures grow beyond 0x400. Only multiboot, GDT
	 * and the coreboot table use low_tables.
	 */
	low_table_start = 0;
	low_table_end = 0x500;

#if CONFIG_GENERATE_PIRQ_TABLE
#define MAX_PIRQ_TABLE_SIZE (4 * 1024)
	post_code(0x9a);

	/* This table must be between 0x0f0000 and 0x100000 */
	rom_table_end = write_pirq_routing_table(rom_table_end);
	rom_table_end = ALIGN(rom_table_end, 1024);

	/* And add a high table version for those payloads that
	 * want to live in the F segment
	 */
	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_PIRQ, MAX_PIRQ_TABLE_SIZE);
	if (high_table_pointer) {
		unsigned long new_high_table_pointer;
		new_high_table_pointer = write_pirq_routing_table(high_table_pointer);
		// FIXME make pirq table code intelligent enough to know how
		// much space it's going to need.
		if (new_high_table_pointer > (high_table_pointer + MAX_PIRQ_TABLE_SIZE)) {
			printk(BIOS_ERR, "ERROR: Increase PIRQ size.\n");
		}
		printk(BIOS_DEBUG, "PIRQ table: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	}

#endif

#if CONFIG_GENERATE_MP_TABLE
#define MAX_MP_TABLE_SIZE (4 * 1024)
	post_code(0x9b);

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	rom_table_end = write_smp_table(rom_table_end);
	rom_table_end = ALIGN(rom_table_end, 1024);

	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_MPTABLE, MAX_MP_TABLE_SIZE);
	if (high_table_pointer) {
		unsigned long new_high_table_pointer;
		new_high_table_pointer = write_smp_table(high_table_pointer);
		// FIXME make mp table code intelligent enough to know how
		// much space it's going to need.
		if (new_high_table_pointer > (high_table_pointer + MAX_MP_TABLE_SIZE)) {
			printk(BIOS_ERR, "ERROR: Increase MP table size.\n");
		}

		printk(BIOS_DEBUG, "MP table: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	}
#endif /* CONFIG_GENERATE_MP_TABLE */

#if CONFIG_GENERATE_ACPI_TABLES
#define MAX_ACPI_SIZE (45 * 1024)
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
	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_ACPI, MAX_ACPI_SIZE);
	if (high_table_pointer) {
		unsigned long acpi_start = high_table_pointer;
		unsigned long new_high_table_pointer;

		rom_table_end = ALIGN(rom_table_end, 16);
		new_high_table_pointer = write_acpi_tables(high_table_pointer);
		if (new_high_table_pointer > ( high_table_pointer + MAX_ACPI_SIZE)) {
			printk(BIOS_ERR, "ERROR: Increase ACPI size\n");
		}
                printk(BIOS_DEBUG, "ACPI tables: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);

		/* Now we need to create a low table copy of the RSDP. */

		/* First we look for the high table RSDP */
		while (acpi_start < new_high_table_pointer) {
			if (memcmp(((acpi_rsdp_t *)acpi_start)->signature, RSDP_SIG, 8) == 0) {
				break;
			}
			acpi_start++;
		}

		/* Now, if we found the RSDP, we take the RSDT and XSDT pointer
		 * from it in order to write the low RSDP
		 */
		if (acpi_start < new_high_table_pointer) {
			acpi_rsdp_t *low_rsdp = (acpi_rsdp_t *)rom_table_end,
				    *high_rsdp = (acpi_rsdp_t *)acpi_start;

			acpi_write_rsdp(low_rsdp,
				(acpi_rsdt_t *)(high_rsdp->rsdt_address),
				(acpi_xsdt_t *)((unsigned long)high_rsdp->xsdt_address));
		} else {
			printk(BIOS_ERR, "ERROR: Didn't find RSDP in high table.\n");
		}
		rom_table_end = ALIGN(rom_table_end + sizeof(acpi_rsdp_t), 16);
	} else {
		rom_table_end = write_acpi_tables(rom_table_end);
		rom_table_end = ALIGN(rom_table_end, 1024);
	}

#endif
#define MAX_SMBIOS_SIZE 2048
#if CONFIG_GENERATE_SMBIOS_TABLES
	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_SMBIOS, MAX_SMBIOS_SIZE);
	if (high_table_pointer) {
		unsigned long new_high_table_pointer;

		new_high_table_pointer = smbios_write_tables(high_table_pointer);
		rom_table_end = ALIGN(rom_table_end, 16);
		memcpy((void *)rom_table_end, (void *)high_table_pointer, sizeof(struct smbios_entry));
		rom_table_end += sizeof(struct smbios_entry);

		if (new_high_table_pointer > ( high_table_pointer + MAX_SMBIOS_SIZE)) {
			printk(BIOS_ERR, "ERROR: Increase SMBIOS size\n");
		}
                printk(BIOS_DEBUG, "SMBIOS tables: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	} else {
		unsigned long new_rom_table_end = smbios_write_tables(rom_table_end);
		printk(BIOS_DEBUG, "SMBIOS size %ld bytes\n", new_rom_table_end - rom_table_end);
		rom_table_end = ALIGN(new_rom_table_end, 16);
	}
#endif

	post_code(0x9e);

#if CONFIG_HAVE_ACPI_RESUME
/* Only add CBMEM_ID_RESUME when the ramstage isn't relocatable. */
#if !CONFIG_RELOCATABLE_RAMSTAGE
	/* Let's prepare the ACPI S3 Resume area now already, so we can rely on
	 * it begin there during reboot time. We don't need the pointer, nor
	 * the result right now. If it fails, ACPI resume will be disabled.
	 */
	cbmem_add(CBMEM_ID_RESUME, HIGH_MEMORY_SAVE);
#endif
#if CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY14 || CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY15_TN || CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY16_KB
	cbmem_add(CBMEM_ID_RESUME_SCRATCH, CONFIG_HIGH_SCRATCH_MEMORY_SIZE);
#endif
#endif

#define MAX_COREBOOT_TABLE_SIZE (32 * 1024)
	post_code(0x9d);

	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_CBTABLE, MAX_COREBOOT_TABLE_SIZE);

	if (high_table_pointer) {
		unsigned long new_high_table_pointer;

		/* FIXME: The high_table_base parameter is not reference when tables are high,
		 * or high_table_pointer >1 MB.
		 */
#if CONFIG_DYNAMIC_CBMEM
		u64 fixme_high_tables_base = 0;
#else
		u64 fixme_high_tables_base = (u32)get_cbmem_toc();
#endif

		/* Also put a forwarder entry into 0-4K */
		new_high_table_pointer = write_coreboot_table(low_table_start, low_table_end,
				fixme_high_tables_base, high_table_pointer);

		if (new_high_table_pointer > (high_table_pointer +
					MAX_COREBOOT_TABLE_SIZE))
			printk(BIOS_ERR, "%s: coreboot table didn't fit (%lx)\n",
				   __func__, new_high_table_pointer -
				   high_table_pointer);

                printk(BIOS_DEBUG, "coreboot table: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	} else {
		/* The coreboot table must be in 0-4K or 960K-1M */
		rom_table_end = write_coreboot_table(
				     low_table_start, low_table_end,
				     rom_table_start, rom_table_end);
	}

#if CONFIG_MULTIBOOT
	post_code(0x9d);

	/* The Multiboot information structure */
	write_multiboot_info(rom_table_end);
#endif

	/* Print CBMEM sections */
	cbmem_list();

	return get_lb_mem();
}
