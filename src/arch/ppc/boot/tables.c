#include <console/console.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include "coreboot_table.h"

struct lb_memory *
write_tables(void)
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

	/* The coreboot table must be in 0-4K or 960K-1M */
	write_coreboot_table(
		low_table_start, low_table_end,
		rom_table_start, rom_table_end);

	return get_lb_mem();
}
