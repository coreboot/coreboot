#include <stdlib.h>
#include <boot/elf.h>
#include <printk.h>
#include <subr.h>
#include <boot/linuxbios_table.h>
#include <arch/smp/smp.h>

#define LINUXBIOS

static unsigned long processor_map[MAX_CPUS];
void early_mainboard_init(void);

void write_tables(struct mem_range *mem)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;
	
	rom_table_start = 0xf0000;
	rom_table_end =   0xf0000;
	/* Start low addr at 16 bytes instead of 0 because of a buglet
	 * in the generic linux bunzip code, as it tests for the a20 line.
	 */
	low_table_start = 0;
	low_table_end = 16;

#ifdef ALPHA_SMP
	/* copy the smp block to address 0 */
	post_code(0x96);
	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	low_table_end = write_smp_table(low_table_end, processor_map);

	/* Don't write anything in the traditional x86 BIOS data segment */
	if (low_table_end < 0x500) {
		low_table_end = 0x500;
	}
#endif
	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(
	    processor_map, mem,
		low_table_start, low_table_end,
		rom_table_start, rom_table_end);
}


void hardwaremain(unsigned long signature, unsigned long memsize, 
	unsigned long cpu_speed)
{
	extern void linuxbiosmain(
	    unsigned long membase, 
	    unsigned long totalram);
	void prepare_hwrpb(unsigned long totalram);
		
	post_code(0x55);
	while(1);

	early_mainboard_init();
	displayinit();

	printk_info("\n\nsignature=0x%016lx memsize=0x%016lx "
	    "cpu_speed=0x%016lx\n",
	    signature, memsize, cpu_speed);

#ifdef OLD_DS10
 	elfboot(memsize >> 10 /* In kilobytes */);
 	printk_err("\n after elfboot\n");
#else
	processor_map[0] = CPU_ENABLED;
	
	/* Set up hwrpb */
	prepare_hwrpb(memsize);
	
	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	write_tables(memsize);
		
	
#ifdef LINUXBIOS
	printk_info("Jumping to linuxbiosmain()...\n");
	// we could go to argc, argv, for main but it seems like overkill.
        post_code(0xed);
	linuxbiosmain(0, memsize);
#endif /* LINUXBIOS */
#endif
}

void fatal_error(unsigned long exception_handler, unsigned long exception_at, 
	unsigned long return_address)
{
	/* Just in case we are totally messed up */
	early_mainboard_init();
	displayinit();
	printk_emerg("\n\nFault: 0x%0lx\n", exception_handler);
	printk_emerg("PC: 0x%0lx\n", exception_at);
	printk_emerg("RA: 0x%0lx\n", return_address);
	/* Now spin forever */
	while(1) {;}
}
