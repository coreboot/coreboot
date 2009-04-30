/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) .... others
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
// 2003-07 by SONE Takeshi
// Ported from Etherboot to coreboot 2005-08 by Steve Magnani
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

#if HAVE_HIGH_TABLES == 1
uint64_t high_tables_base = 0;
uint64_t high_tables_size;
#endif

struct lb_memory *write_tables(void)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;
#if HAVE_MP_TABLE == 1 && HAVE_LOW_TABLES == 1
	unsigned long new_low_table_end;
#endif

#if HAVE_HIGH_TABLES == 1
	/* Even if high tables are configured, all tables are copied both to the
	 * low and the high area, so payloads and OSes don't need to know about
	 * the high tables.
	 */
	unsigned long high_table_start=0, high_table_end=0;

	if (high_tables_base) {
		printk_debug("High Tables Base is %llx.\n", high_tables_base);
		high_table_start = high_tables_base;
		high_table_end = high_tables_base;
	} else {
		printk_debug("High Tables Base is not set.\n");
	}
#endif

	rom_table_start = 0xf0000; 
	rom_table_end =   0xf0000;
	/* Start low addr at 16 bytes instead of 0 because of a buglet
	 * in the generic linux unzip code, as it tests for the a20 line.
	 */
	low_table_start = 0;
	low_table_end = 16;

	post_code(0x9a);

#if HAVE_LOW_TABLES == 1
	/* This table must be betweeen 0xf0000 & 0x100000 */
	rom_table_end = write_pirq_routing_table(rom_table_end);
	rom_table_end = (rom_table_end + 1023) & ~1023;
#endif
#if HAVE_HIGH_TABLES == 1
	if (high_tables_base) {
		high_table_end = write_pirq_routing_table(high_table_end);
		high_table_end = (high_table_end + 1023) & ~1023;
	}
#endif

	/* Write ACPI tables */
	/* write them in the rom area because DSDT can be large (8K on epia-m) which
	 * pushes coreboot table out of first 4K if set up in low table area 
	 */
#if HAVE_ACPI_TABLES == 1
#if HAVE_HIGH_TABLES == 1
#if HAVE_LOW_TABLES == 1
	unsigned long high_rsdp=ALIGN(high_table_end, 16);
	unsigned long rsdt_location=(unsigned long*)(((acpi_rsdp_t*)high_rsdp)->rsdt_address);
	acpi_write_rsdp(rom_table_end, rsdt_location);
	rom_table_end = ALIGN(ALIGN(rom_table_end, 16) + sizeof(acpi_rsdp_t), 16);
#endif
	if (high_tables_base) {
		high_table_end = write_acpi_tables(high_table_end);
		high_table_end = (high_table_end+1023) & ~1023;
	}
#else
#if HAVE_LOW_TABLES == 1
	rom_table_end = write_acpi_tables(rom_table_end);
	rom_table_end = (rom_table_end+1023) & ~1023;
#endif
#endif
#endif
	/* copy the smp block to address 0 */
	post_code(0x96);

#if HAVE_MP_TABLE == 1

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
#if HAVE_LOW_TABLES == 1
	new_low_table_end = write_smp_table(low_table_end); // low_table_end is 0x10 at this point
        /* Don't write anything in the traditional x86 BIOS data segment,
         * for example the linux kernel smp need to use 0x467 to pass reset vector
         * or use 0x40e/0x413 for EBDA finding...
         */
	if(new_low_table_end>0x400){
		unsigned mptable_size;
		unsigned mpc_start;
		low_table_end += SMP_FLOATING_TABLE_LEN; /* keep the mpf in 1k low, so kernel can find it */
		mptable_size = new_low_table_end - low_table_end;
		/* We can not put mptable low, we need to copy them to somewhere else*/
		if((rom_table_end+mptable_size)<0x100000) {
			/* We can copy mptable on rom_table  */
			mpc_start = rom_table_end;
			rom_table_end += mptable_size;
			rom_table_end = (rom_table_end+1023) & ~1023;
		} else {
			/* We can need to put mptable before rom_table */
			mpc_start = rom_table_start - mptable_size;
			mpc_start &= ~1023;
			rom_table_start = mpc_start;
		}
		printk_debug("move mptable from 0x%0lx to 0x%0x, size 0x%0x\n", low_table_end, mpc_start, mptable_size);
		memcpy((unsigned char *)mpc_start, (unsigned char *)low_table_end, mptable_size);
		smp_write_floating_table_physaddr(low_table_end - SMP_FLOATING_TABLE_LEN, mpc_start);
		memset((unsigned char *)low_table_end, '\0', mptable_size);
	}
#endif /* HAVE_LOW_TABLES */

#if HAVE_HIGH_TABLES == 1
	if (high_tables_base) {
		high_table_end = write_smp_table(high_table_end);
		high_table_end = (high_table_end+1023) & ~1023;
	}
#endif
#endif /* HAVE_MP_TABLE */

	if (low_table_end < 0x500) {
		low_table_end = 0x500;
	}

	// Relocate the GDT to reserved memory, so it won't get clobbered
#if HAVE_HIGH_TABLES == 1
	if (high_tables_base) {
		move_gdt(high_table_end);
		high_table_end += &gdt_end - &gdt;
		high_table_end = (high_table_end+1023) & ~1023;
	} else {
#endif
		move_gdt(low_table_end);
		low_table_end += &gdt_end - &gdt;
#if HAVE_HIGH_TABLES == 1
	}
#endif

#if CONFIG_MULTIBOOT
	/* The Multiboot information structure */
	mbi = (struct multiboot_info *)rom_table_end;
	rom_table_end = write_multiboot_info(
				low_table_start, low_table_end,
				rom_table_start, rom_table_end);
#endif

#if HAVE_HIGH_TABLES == 1
	if (high_tables_base) {
		write_coreboot_table(low_table_start, low_table_end,
				high_table_start, high_table_end);
	} else {
		printk_err("ERROR: No high_tables_base.\n");
		write_coreboot_table(low_table_start, low_table_end,
			      rom_table_start, rom_table_end);
	}
#else
	/* The coreboot table must be in 0-4K or 960K-1M */
	write_coreboot_table(low_table_start, low_table_end,
			      rom_table_start, rom_table_end);
#endif
 
	return get_lb_mem();
}
