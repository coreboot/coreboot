/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */


/*
 * C Bootstrap code for the INTEL 
 * $Id$
 *
 */

#define LINUXBIOS
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <arch/io.h>
#include <arch/intel.h>
#include <pciconf.h>
#include <pci.h>
#include <subr.h>
#include <printk.h>
#include <smp/start_stop.h>
#include <cpu/l2_cache.h>
#include <cpu/cpufixup.h>
#include <cpu/p5/cpuid.h>
#include <part/mainboard.h>
#include <part/keyboard.h>
#include <part/framebuffer.h>
#include <part/nvram.h>
#include <part/floppy.h>
#include <part/sizeram.h>
#include <part/hard_reset.h>
#include <arch/ioapic.h>
#include <pc80/i8259.h>
#include <pc80/keyboard.h>
#include <arch/i386_subr.h>
#include <arch/pirq_routing.h>
#include <arch/ioapic.h>
#include <smp/atomic.h>
#include <arch/smp/mpspec.h>
#include <boot/linuxbios_table.h>


/* The processor map. 
 * Now that SMP is in linuxbios, and Linux counts on us
 * giving accurate information about processors, we need a map
 * of what processors are out there. This could be a bit mask, 
 * but we will be optimistic and hope we someday run on 
 * REALLY BIG SMPs. Also we may need more than one bit of 
 * info per processor at some point. I hope we don't need 
 * anything more complex than an int.
 */
static unsigned long processor_map[MAX_CPUS];

static unsigned long cpu_initialize(unsigned long totalram)
{
	/* Because we busy wait at the printk spinlock.
	 * It is important to keep the number of printed messages
	 * from secondary cpus to a minimum, when debugging is
	 * disabled.
	 */
	unsigned long processor_id = this_processors_id();
	printk_notice("Initializing CPU #%d\n", processor_id);
	
	/* some cpus need a fixup done. This is the hook for doing that. */
	cpufixup(totalram);

	/* Turn on caching if we haven't already */
	cache_on(totalram);

	display_cpuid();
	mtrr_check();

	/* now that everything is really up, enable the l2 cache if desired. 
	 * The enable can wait until this point, because linuxbios and it's
	 * data areas are tiny, easily fitting into the L1 cache. 
	 */
	configure_l2_cache();
	interrupts_on();
	printk_info("CPU #%d Initialized\n", processor_id);
	return processor_id;
}

static unsigned long get_ramsize(void)
{
	unsigned long totalram;
	totalram = sizeram();
	// can't size just yet ... 
	// mainboard totalram sizing may not be up yet. If it is not ready, 
	// take a default of 64M
	if (!totalram)
		totalram = 64 * 1024;
	return totalram;
}

#ifdef SMP
/* Number of cpus that are currently running in linuxbios */
static atomic_t active_cpus = ATOMIC_INIT(1);

void secondary_cpu_init(void)
{
	unsigned long totalram;
	unsigned long id;
	int index;

	atomic_inc(&active_cpus);
	printk_debug(__FUNCTION__ "\n");
	totalram = get_ramsize();
	id = cpu_initialize(totalram);
	index = processor_index(id);
	printk_debug(__FUNCTION__ "  %d/%u\n", index, id);
	processor_map[index] = CPU_ENABLED;
	atomic_dec(&active_cpus);
	stop_cpu(id);
}

static void wait_for_other_cpus(void)
{
	int old_active_count, active_count;
	int i;
	old_active_count = 1;
	active_count = atomic_read(&active_cpus);
	while(active_count > 1) {
		if (active_count != old_active_count) {
			printk_info("Waiting for %d CPUS to stop\n", active_count);
			old_active_count = active_count;
		}
		active_count = atomic_read(&active_cpus);
	}
	for(i = 0; i < MAX_CPUS; i++) {
		if (!(processor_map[i] & CPU_ENABLED)) {
			printk_err("CPU %d/%u did not initialize!\n",
				i, initial_apicid);
		}
	}
	printk_debug("All AP CPUs stopped\n");
}
#else /* SMP */
#define wait_for_other_cpus() do {} while(0)
#endif /* SMP */

void write_tables(unsigned long totalram)
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

	post_code(0x9a);
	check_pirq_routing_table();
	/* This table must be betweeen 0xf0000 & 0x100000 */
	rom_table_end = copy_pirq_routing_table(rom_table_end);

	/* copy the smp block to address 0 */
	post_code(0x96);
	/* The smp table must be in 0-1K, 639K-640K, or 960K-1M */
	low_table_end = write_smp_table(low_table_end, processor_map);

	/* Don't write anything in the traditional x86 BIOS data segment */
	if (low_table_end < 0x500) {
		low_table_end = 0x500;
	}
	/* The linuxbios table must be in 0-4K or 960K-1M */
	write_linuxbios_table(
		processor_map, totalram,
		low_table_start, low_table_end,
		rom_table_start, rom_table_end);
}

void hardwaremain(int boot_complete)
{
	/* Processor ID of the BOOT cpu (i.e. the one running this code) */
	unsigned long boot_cpu;
	int boot_index;

	/* Comment: the NEW_SUPERIO architecture is actually pretty good.
	 * I think we need to move to the same sort of architecture for
	 * everything: A config file generated sequence of calls 
	 * for initializing all the chips. We stick with this 
	 * for now -- rgm. 
	 */
#ifdef USE_NEW_SUPERIO_INTERFACE
	extern struct superio *all_superio;
	extern int nsuperio;
	extern void handle_superio(int pass, struct superio *s, int nsuperio);
#endif
	/* the order here is a bit tricky. We don't want to do much of 
	 * anything that uses config registers until after PciAllocateResources
	 * since that function also figures out what kind of config strategy
	 * to use (type 1 or type 2). 
	 * so we turn on cache, then worry about PCI setup, then do other 
	 * things, so that the other work can use the PciRead* and PciWrite*
	 * functions. 
	 */
	unsigned long totalram = 0;
	extern void linuxbiosmain(unsigned long membase, unsigned long totalram);

	/* displayinit MUST PRECEDE ALL PRINTK! */
	displayinit();
	
	post_code(0x39);

	printk_notice("LinuxBIOS %s...\n", (boot_complete)?"rebooting":"booting");

	post_code(0x40);

	/* If we have already booted attempt a hard reboot */
	if (boot_complete) {
		hard_reset();
	}

	// pick how to scan the bus. This is first so we can get at memory size.
	printk_info("Finding PCI configuration type.\n");
	pci_set_method();
	post_code(0x5f);
#ifdef USE_NEW_SUPERIO_INTERFACE
	handle_superio(0, all_superio, nsuperio);
#endif
	pci_enumerate();
	post_code(0x66);

	// The framebuffer can change how much memory you have. 
	// So you really need to run this before you size ram. 
	framebuffer_on();

	totalram = get_ramsize();
	post_code(0x70);
	printk_info("totalram: %ldM\n", totalram/1024);

	/* Fully initialize the cpu before configuring the bus */
	boot_cpu = cpu_initialize(totalram);
	boot_index = processor_index(boot_cpu);
	printk_spew("BOOT CPU is %d\n", boot_cpu);
	processor_map[boot_index] = CPU_BOOTPROCESSOR|CPU_ENABLED;

	/* Now start the other cpus initializing 
	 * The sooner they start the sooner they stop.
	 */
	post_code(0x75);
	startup_other_cpus(processor_map);
	post_code(0x77);

	// Now do the real bus
	// we round the total ram up a lot for thing like the SISFB, which 
	// shares high memory with the CPU. 
	pci_configure();
	post_code(0x88);

	pci_enable();
	post_code(0x90);

	// generic mainboard fixup
	mainboard_fixup();

#ifndef MAINBOARD_FIXUP_IN_CHARGE

	post_code(0x91);
	setup_i8259();

	/* set up the IO-APIC for the clock interrupt. */
	post_code(0x92);
	setup_ioapic();

	nvram_on();

	keyboard_on();

#ifndef USE_NEW_SUPERIO_INTERFACE
	enable_floppy();
	post_code(0x95);
#endif

#ifdef USE_NEW_SUPERIO_INTERFACE
	handle_superio(1, all_superio, nsuperio);
#endif

	pci_zero_irq_settings();



	/* to do: intel_serial_on(); */

	final_mainboard_fixup();
	post_code(0xec);


#endif /* MAINBOARD_FIXUP_IN_CHARGE */

#ifdef USE_NEW_SUPERIO_INTERFACE
	handle_superio(2, all_superio, nsuperio);
#endif
	/* make certain we are the only cpu running in linuxBIOS */
	wait_for_other_cpus();

	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	write_tables(totalram);


#ifdef LINUXBIOS
	printk_info("Jumping to linuxbiosmain()...\n");
	// we could go to argc, argv, for main but it seems like overkill.
	post_code(0xed);
	linuxbiosmain(0, totalram);
#endif /* LINUXBIOS */
}


