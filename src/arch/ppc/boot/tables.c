#include <console/console.h>
#include <mem.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/linuxbios_tables.h>
#include "linuxbios_table.h"

struct lb_memory *
write_tables(struct mem_range *mem, unsigned long *processor_map)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;

	rom_table_start = 0xf0000;      
	rom_table_end =   0xf0000;           
	/* Start low addr at 16 bytes instead of 0 because of a buglet
	* in the generic linux unzip code, as it tests for the a20 line.
	*/
	low_table_start = 0;
	low_table_end = 16;

#if 0
	post_code(0x9a);
	check_pirq_routing_table();
	/* This table must be betweeen 0xf0000 & 0x100000 */
	rom_table_end = copy_pirq_routing_table(rom_table_end);
	rom_table_end = (rom_table_end + 1023) & ~1023;

	/* copy the smp block to address 0 */
	post_code(0x96);
	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	remove_logical_cpus();
	low_table_end = write_smp_table(low_table_end, processor_map);

	/* Don't write anything in the traditional x86 BIOS data segment */
	if (low_table_end < 0x500) {
	low_table_end = 0x500;
	}
#endif
	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(processor_map, mem,
				low_table_start, low_table_end,
				rom_table_start >> 10, rom_table_end >> 10);

	return get_lb_mem();
}
