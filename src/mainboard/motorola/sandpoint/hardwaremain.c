/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <ppc.h>
#include <bsp.h>
#include <printk.h>
#include <subr.h>
#include <pci.h>
#include <mem.h>
#include <version.h>
#include <part/hard_reset.h>
#include <boot/elf.h>
#include <rom/read_bytes.h>
#include <northbridge/motorola/mpc107/epic.h>
#include "nvram.h"

#define CPU_ENABLED             1       /* Processor is available */
#define CPU_BOOTPROCESSOR       2       /* Processor is the BP */

static unsigned long processor_map[MAX_CPUS];

extern unsigned long memory_base;
extern unsigned long memory_size;

extern struct superio *all_superio[];    
extern int nsuperio;
extern void handle_superio(int pass, struct superio *s[], int nsuperio);

extern nvram_device bsp_nvram;
extern int init_flash_amd800(char *, unsigned, unsigned);
extern void southbridge_early_init(void);
extern void southbridge_init(void);
extern void sio_enable(void);
extern unsigned long this_processors_id(void);
extern unsigned long processor_index(unsigned long);
extern void wait_for_other_cpus(void);
extern void startup_other_cpus(unsigned long *);
extern struct mem_range *getmeminfo(void);

extern struct pci_ops pci_direct_ppc;

void
testram(struct mem_range *mem)
{
	unsigned addr;
	unsigned val;
	int errors = 0;

	for ( addr = 0x0 ; addr < 0x100000; addr++)
	{
		val = *((unsigned *)addr);
		*((unsigned *)addr) = addr;

		if (*((unsigned *)addr) != addr)
			errors++;

		*((unsigned *)addr) = val;
	}

	if ( errors )
		printk_info("%d errors found\n", errors);
	else
		printk_info("no errors found\n");
}

static unsigned long 
cpu_initialize(struct mem_range *mem)
{
        /* Because we busy wait at the printk spinlock.
	 * It is important to keep the number of printed messages
	 * from secondary cpus to a minimum, when debugging is
	 * disabled.
	 */
	unsigned long processor_id = this_processors_id();
	printk_notice("Initializing CPU #%d\n", processor_id);

#if 0
	/* Turn on caching if we haven't already */
	cache_on(mem);
#endif

	ppc_identify();

#if 0
	/* now that everything is really up, enable the l2 cache if desired.
	* The enable can wait until this point, because linuxbios and it's
	* data areas are tiny, easily fitting into the L1 cache.
	*/
	configure_l2_cache();
	interrupts_on();
#endif
	printk_info("CPU #%d Initialized\n", processor_id);
	return processor_id;
}

#if 0
void
write_tables(struct mem_range *mem)
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
	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(processor_map, mem,
				low_table_start, low_table_end,
				rom_table_start >> 10, rom_table_end >> 10);
}
#endif

void
hardwaremain(int boot_complete)
{
	struct mem_range *mem, *tmem;
	unsigned long totalmem;
	unsigned long boot_cpu;
	int boot_index;

#ifdef PPC
	//southbridge_early_init();
	sio_enable();
#else /* PPC */
	// we don't call post code for this one -- since serial post could cause real

	// trouble.
	outb(0x38, 0x80);
#endif /* PPC */

	/* displayinit MUST PRECEDE ALL PRINTK! */
	displayinit();

	post_code(0x39);
	printk_notice("LinuxBIOS-%s%s %s %s...\n", 
		linuxbios_version, linuxbios_extra_version, linuxbios_build,
		(boot_complete)?"rebooting":"booting");

	post_code(0x40);

	/* If we have already booted attempt a hard reboot */
	if (boot_complete) {
		hard_reset();
	}

	// pick how to scan the bus. This is first so we can get at memory size.

	printk_info("Finding PCI configuration type.\n");
#ifdef PPC
	if ( !pci_set_direct(&pci_direct_ppc) )
	{
		printk_info("Could not access PCI bus\n");
		return;    
	}
#else /* PPC */
	pci_set_method();
#endif /* PPC */

	post_code(0x5f);
#if 0
	enumerate_static_devices();
#endif
	pci_enumerate();
	post_code(0x66);
	// Now do the real bus
	// we round the total ram up a lot for thing like the SISFB, which 
	// shares high memory with the CPU. 
	pci_configure();
	post_code(0x88);

	pci_enable();
	pci_initialize();
	post_code(0x89);

#ifdef PPC
	mem = getmeminfo();
#else /* PPC */
	mem = get_ramsize();
#endif /* PPC */
	post_code(0x70);
	totalmem = 0;
	for(tmem = mem; tmem->sizek; tmem++) {
		totalmem += tmem->sizek;
	}
	printk_info("totalram: %ldM\n", 
		(totalmem + 512) >> 10); /* Round to the nearest meg */

	/* Fully initialize the cpu before configuring the bus */
	boot_cpu = cpu_initialize(mem);
	boot_index = processor_index(boot_cpu);
	printk_spew("BOOT CPU is %d\n", boot_cpu);
	processor_map[boot_index] = CPU_BOOTPROCESSOR|CPU_ENABLED;

	/* Now start the other cpus initializing 
	 * The sooner they start the sooner they stop.
	 */
	post_code(0x75);
	startup_other_cpus(processor_map);
	post_code(0x77);

	/* make certain we are the only cpu running in linuxBIOS */
	wait_for_other_cpus();

#ifdef PPC
	init_flash_amd800("BOOT", 0xff000000, 1);
	init_flash_amd800("BOOT", 0xff800000, 1);

	southbridge_init();

	nvram_init(&bsp_nvram);

	handle_superio(0, all_superio, nsuperio);
	handle_superio(1, all_superio, nsuperio);
	handle_superio(2, all_superio, nsuperio);

	/*
	 * Initialise interrupt router
	 */
	epicInit(EPIC_SERIAL_IRQ, 1);
#endif /* PPC */

#if 0
	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	write_tables(mem);
#endif

	elfboot(streams, get_lb_mem());

	printk_debug("Halting...\n");
	for (;;);
}
