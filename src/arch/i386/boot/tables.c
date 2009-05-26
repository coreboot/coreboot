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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* 2006.1 yhlu add mptable cross 0x467 processing */

#include <console/console.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <arch/pirq_routing.h>
#include <arch/smp/mpspec.h>
#include <arch/acpi.h>
#include <string.h>
#include <cpu/x86/multiboot.h>
#include "coreboot_table.h"

// Global Descriptor Table, defined in c_start.S
extern uint8_t gdt;
extern uint8_t gdt_end;

/* i386 lgdt argument */
struct gdtarg {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

// Copy GDT to new location and reload it
void move_gdt(unsigned long newgdt)
{
	uint16_t num_gdt_bytes = &gdt_end - &gdt;
	struct gdtarg gdtarg;

	printk_debug("Moving GDT to %#lx...", newgdt);
	memcpy((void*)newgdt, &gdt, num_gdt_bytes);
	gdtarg.base = newgdt;
	gdtarg.limit = num_gdt_bytes - 1;
	__asm__ __volatile__ ("lgdt %0\n\t" : : "m" (gdtarg));
	printk_debug("ok\n");
}

uint64_t high_tables_base = 0;
uint64_t high_tables_size;

struct lb_memory *write_tables(void)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;
#if HAVE_MP_TABLE == 1
	unsigned long new_low_table_end;
#endif

	/* Even if high tables are configured, some tables are copied both to
	 * the low and the high area, so payloads and OSes don't need to know
	 * about the high tables.
	 */
	unsigned long high_rsdp;
	unsigned long high_table_start=0, high_table_end=0;

	if (high_tables_base) {
		printk_debug("High Tables Base is %llx.\n", high_tables_base);
		high_table_start = high_tables_base;
		high_table_end = high_tables_base;
	} else {
		printk_err("ERROR: High Tables Base is not set.\n");
	}

	rom_table_start = 0xf0000; 
	rom_table_end =   0xf0000;

	/* Start low addr at 0x500, so we don't run into conflicts with the BDA
	 * in case our data structures grow beyound 0x400. Only multiboot, GDT
	 * and the coreboot table use low_tables. 
	 */
	low_table_start = 0;
	low_table_end = 0x500;

	post_code(0x99);

	/* This table must be between 0x0f0000 and 0x100000 */
	rom_table_end = write_pirq_routing_table(rom_table_end);
	rom_table_end = ALIGN(rom_table_end, 1024);

	/* And add a high table version for those payloads that
	 * want to live in the F segment
	 */
	if (high_tables_base) {
		high_table_end = write_pirq_routing_table(high_table_end);
		high_table_end = ALIGN(high_table_end, 1024);
	}

	post_code(0x9a);

	/* Write ACPI tables to F segment and high tables area */
#if HAVE_ACPI_TABLES == 1
	if (high_tables_base) {
		unsigned long rsdt_location;
		high_rsdp = ALIGN(high_table_end, 16);
		high_table_end = write_acpi_tables(high_table_end);
		high_table_end = ALIGN(high_table_end, 1024);
		rsdt_location = (unsigned long)(((acpi_rsdp_t*)high_rsdp)->rsdt_address);
		printk_debug("high mem RSDP at %x, RSDT at %x\n", high_rsdp, rsdt_location);
		acpi_write_rsdp((acpi_rsdp_t *)rom_table_end, (acpi_rsdt_t *)rsdt_location);
		rom_table_end = ALIGN(ALIGN(rom_table_end, 16) + sizeof(acpi_rsdp_t), 16);
	} else {
		rom_table_end = write_acpi_tables(rom_table_end);
		rom_table_end = ALIGN(rom_table_end, 1024);
	}
#endif
	post_code(0x9b);

#if HAVE_MP_TABLE == 1
	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	rom_table_end = write_smp_table(rom_table_end);
	rom_table_end = ALIGN(rom_table_end, 1024);

	/* ... and a copy in the high tables */
	if (high_tables_base) {
		high_table_end = write_smp_table(high_table_end);
		high_table_end = ALIGN(high_table_end, 1024);
	}
#endif /* HAVE_MP_TABLE */

	post_code(0x9c);

	// Relocate the GDT to reserved memory, so it won't get clobbered
	if (high_tables_base) {
		move_gdt(high_table_end);
		high_table_end += &gdt_end - &gdt;
		high_table_end = ALIGN(high_table_end, 1024);
	} else {
		move_gdt(low_table_end);
		low_table_end += &gdt_end - &gdt;
	}

	post_code(0x9d);

#if CONFIG_MULTIBOOT
	/* The Multiboot information structure */
	rom_table_end = write_multiboot_info(
				low_table_start, low_table_end,
				rom_table_start, rom_table_end);
#endif

	post_code(0x9e);

	if (high_tables_base) {
		/* Also put a forwarder entry into 0-4K */
		write_coreboot_table(low_table_start, low_table_end,
				high_table_start, high_table_end);
	} else {
		/* The coreboot table must be in 0-4K or 960K-1M */
		write_coreboot_table(low_table_start, low_table_end,
			      rom_table_start, rom_table_end);
	}
 
	post_code(0x9f);

	return get_lb_mem();
}
