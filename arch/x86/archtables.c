/*
 * table management code for Linux BIOS
 *
 *
 * Copyright (C) 2002 Eric Biederman, Linux NetworX
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */

/* 2006.1 yhlu add mptable cross 0x467 processing */

#include <types.h>
#include <console.h>
#include <string.h>
#include <tables.h>
//#include <cpu/cpu.h>
//#include <pirq_routing.h>
//#include <smp/mpspec.h>
//#include <acpi.h>

// Global Descriptor Table, defined in c_start.S
extern u8 gdt;
extern u8 gdt_end;

/* i386 lgdt argument */
struct gdtarg {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

#warning enable disabled code in archtables.c

#if 0
// Copy GDT to new location and reload it
// 2003-07 by SONE Takeshi
// Ported from Etherboot to LinuxBIOS 2005-08 by Steve Magnani
void move_gdt(unsigned long newgdt)
{
	u16 num_gdt_bytes = &gdt_end - &gdt;
	struct gdtarg gdtarg;

	printk(BIOS_DEBUG,"Moving GDT to %#lx...", newgdt);
	memcpy((void*)newgdt, &gdt, num_gdt_bytes);
	gdtarg.base = newgdt;
	gdtarg.limit = num_gdt_bytes - 1;
	__asm__ __volatile__ ("lgdt %0\n\t" : : "m" (gdtarg));
	printk(BIOS_DEBUG,"OK\n");
}
#endif
struct lb_memory *arch_write_tables(void)
{
#if 0
#if HAVE_MP_TABLE==1
	unsigned long new_low_table_end;
#endif
#endif
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;

	rom_table_start = 0xf0000;
	rom_table_end =   0xf0000;
	/* Start low addr at 16 bytes instead of 0 because of a buglet
	 * in the generic linux unzip code, as it tests for the a20 line.
	 */
	low_table_start = 0;
	low_table_end = 16;

	post_code(POST_STAGE2_ARCH_WRITE_TABLES_ENTER);

	/* This table must be betweeen 0xf0000 & 0x100000 */
//	rom_table_end = write_pirq_routing_table(rom_table_end);
//	rom_table_end = (rom_table_end + 1023) & ~1023;

	/* Write ACPI tables */
	/* write them in the rom area because DSDT can be large (8K on epia-m) which
	 * pushes linuxbios table out of first 4K if set up in low table area 
         */

//	rom_table_end = write_acpi_tables(rom_table_end);
//	rom_table_end = (rom_table_end+1023) & ~1023;

	/* copy the smp block to address 0 */
	post_code(POST_STAGE2_ARCH_WRITE_TABLES_MIDDLE);

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
//	new_low_table_end = write_smp_table(low_table_end);
#if 0
#if HAVE_MP_TABLE==1
        /* Don't write anything in the traditional x86 BIOS data segment,
         * for example the linux kernel smp need to use 0x467 to pass reset vector
         */
        if(new_low_table_end>0x467){
                unsigned mptable_size = new_low_table_end - low_table_end - SMP_FLOATING_TABLE_LEN;
                /* We can not put mptable here, we need to copy them to somewhere else*/
                if((rom_table_end+mptable_size)<0x100000) {
                        /* We can copy mptable on rom_table, and leave low space for lbtable  */
                        printk(BIOS_DEBUG,"Move mptable to 0x%0x\n", rom_table_end);
                        memcpy((unsigned char *)rom_table_end, (unsigned char *)(low_table_end+SMP_FLOATING_TABLE_LEN), mptable_size);
                        memset((unsigned char *)low_table_end, '\0', mptable_size + SMP_FLOATING_TABLE_LEN);
                        smp_write_floating_table_physaddr(low_table_end, rom_table_end);
                        low_table_end += SMP_FLOATING_TABLE_LEN;
                        rom_table_end += mptable_size;
                        rom_table_end = (rom_table_end+1023) & ~1023;
                } else {
                        /* We can need to put mptable low and from 0x500 */
                        printk(BIOS_DEBUG,"Move mptable to 0x%0x\n", 0x500);
                        memcpy((unsigned char *)0x500, (unsigned char *)(low_table_end+SMP_FLOATING_TABLE_LEN), mptable_size);
                        memset((unsigned char *)low_table_end, '\0', 0x500-low_table_end);
                        smp_write_floating_table_physaddr(low_table_end, 0x500);
                        low_table_end = 0x500 + mptable_size;
                }
        }
#endif 
#endif

	/* Don't write anything in the traditional x86 BIOS data segment */
	if (low_table_end < 0x500) {
		low_table_end = 0x500;
	}

#warning GDT should be placed in a reserved position from the beginning on.
#if 0
	// Relocate the GDT to reserved memory, so it won't get clobbered
	move_gdt(low_table_end);
	low_table_end += &gdt_end - &gdt;
#endif
	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(
			      low_table_start, low_table_end,
			      rom_table_start, rom_table_end);

	return get_lb_mem();
}
