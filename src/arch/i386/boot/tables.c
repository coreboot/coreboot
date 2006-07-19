
/* 2006.1 yhlu add mptable cross 0x467 processing */

#include <console/console.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/linuxbios_tables.h>
#include <arch/pirq_routing.h>
#include <arch/smp/mpspec.h>
#include <arch/acpi.h>
#include "linuxbios_table.h"

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
// Ported from Etherboot to LinuxBIOS 2005-08 by Steve Magnani
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

struct lb_memory *write_tables(void)
{
	unsigned long low_table_start, low_table_end, new_low_table_end;
	unsigned long rom_table_start, rom_table_end;

	rom_table_start = 0xf0000;
	rom_table_end =   0xf0000;
	/* Start low addr at 16 bytes instead of 0 because of a buglet
	 * in the generic linux unzip code, as it tests for the a20 line.
	 */
	low_table_start = 0;
	low_table_end = 16;

	post_code(0x9a);

	/* This table must be betweeen 0xf0000 & 0x100000 */
	rom_table_end = write_pirq_routing_table(rom_table_end);
	rom_table_end = (rom_table_end + 1023) & ~1023;

	/* Write ACPI tables */
	/* write them in the rom area because DSDT can be large (8K on epia-m) which
	 * pushes linuxbios table out of first 4K if set up in low table area 
         */

	rom_table_end = write_acpi_tables(rom_table_end);
	rom_table_end = (rom_table_end+1023) & ~1023;

	/* copy the smp block to address 0 */
	post_code(0x96);

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	new_low_table_end = write_smp_table(low_table_end);

        /* Don't write anything in the traditional x86 BIOS data segment,
         * for example the linux kernel smp need to use 0x467 to pass reset vector
         */
        if(new_low_table_end>0x467){
                unsigned mptable_size = new_low_table_end - low_table_end - SMP_FLOATING_TABLE_LEN;
                /* We can not put mptable here, we need to copy them to somewhere else*/
                if((rom_table_end+mptable_size)<0x100000) {
                        /* We can copy mptable on rom_table, and leave low space for lbtable  */
                        printk_debug("move mptable to 0x%0x\n", rom_table_end);
                        memcpy((unsigned char *)rom_table_end, (unsigned char *)(low_table_end+SMP_FLOATING_TABLE_LEN), mptable_size);
                        memset((unsigned char *)low_table_end, '\0', mptable_size + SMP_FLOATING_TABLE_LEN);
                        smp_write_floating_table_physaddr(low_table_end, rom_table_end);
                        low_table_end += SMP_FLOATING_TABLE_LEN;
                        rom_table_end += mptable_size;
                        rom_table_end = (rom_table_end+1023) & ~1023;
                } else {
                        /* We can need to put mptable low and from 0x500 */
                        printk_debug("move mptable to 0x%0x\n", 0x500);
                        memcpy((unsigned char *)0x500, (unsigned char *)(low_table_end+SMP_FLOATING_TABLE_LEN), mptable_size);
                        memset((unsigned char *)low_table_end, '\0', 0x500-low_table_end);
                        smp_write_floating_table_physaddr(low_table_end, 0x500);
                        low_table_end = 0x500 + mptable_size;
                }
        }

	/* Don't write anything in the traditional x86 BIOS data segment */
	if (low_table_end < 0x500) {
		low_table_end = 0x500;
	}

	// Relocate the GDT to reserved memory, so it won't get clobbered
	move_gdt(low_table_end);
	low_table_end += &gdt_end - &gdt;

	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(
			      low_table_start, low_table_end,
			      rom_table_start, rom_table_end);

	return get_lb_mem();
}
