#include <console/console.h>
#include <mem.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/linuxbios_tables.h>
#include <arch/pirq_routing.h>
#include <arch/smp/mpspec.h>
#include <arch/acpi.h>
#include <pc80/mc146818rtc.h>
#include "linuxbios_table.h"

#if CONFIG_SMP && CONFIG_MAX_PHYSICAL_CPUS && (CONFIG_MAX_PHYSICAL_CPUS < CONFIG_MAX_CPUS)
static void remove_logical_cpus(unsigned long *processor_map)
{
	/* To turn off hyperthreading just remove the logical
	 * cpus from the processor map.
	 */
	int disable_logical_cpus = !CONFIG_LOGICAL_CPUS;
	if (get_option(&disable_logical_cpus,"hyper_threading")) {
		disable_logical_cpus = !CONFIG_LOGICAL_CPUS;
	}
	if (disable_logical_cpus) {
		/* disable logical cpus */
		int cnt;
		for(cnt=CONFIG_MAX_PHYSICAL_CPUS;cnt<CONFIG_MAX_CPUS;cnt++)
			processor_map[cnt]=0;
		printk_debug("logical cpus disabled\n");
	}
}
#else

#define remove_logical_cpus(processor_map) do {} while(0) 

#endif /* CONFIG_SMP && CONFIG_MAX_PHYSICAL_CPUS */

struct lb_memory *write_tables(struct mem_range *mem, unsigned long *processor_map)
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

	post_code(0x9a);

	/* This table must be betweeen 0xf0000 & 0x100000 */
	rom_table_end = copy_pirq_routing_table(rom_table_end);
	rom_table_end = (rom_table_end + 1023) & ~1023;

	/* copy the smp block to address 0 */
	post_code(0x96);

	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	remove_logical_cpus(processor_map);
	low_table_end = write_smp_table(low_table_end, processor_map);

	/* Write ACPI tables */
	/* write them in the rom area because DSDT can be large (8K on epia-m) which
	 * pushes linuxbios table out of first 4K if set up in low table area 
         */

	rom_table_end = write_acpi_tables(rom_table_end);
	rom_table_end = (rom_table_end+1023) & ~1023;

	/* Don't write anything in the traditional x86 BIOS data segment */
	if (low_table_end < 0x500) {
		low_table_end = 0x500;
	}

	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(processor_map, mem,
			      low_table_start, low_table_end,
			      rom_table_start >> 10, rom_table_end >> 10);

	return get_lb_mem();
}
