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
 * C Bootstrap code for the LinuxBIOS
 */


#include <console/console.h>
#include <cpu/cpu.h>
#include <mem.h>
#include <version.h>
#include <smp/start_stop.h>
#include <boot/tables.h>
#include <part/sizeram.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/chip.h>
#include <delay.h>
#if 0
#include <part/mainboard.h>
#endif
#if 0
#include <part/hard_reset.h>
#endif
#include <smp/atomic.h>
#include <boot/elf.h>


#ifndef CONFIG_MAX_PHYSICAL_CPUS
#define CONFIG_MAX_PHYSICAL_CPUS CONFIG_MAX_CPUS
#endif

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

static struct mem_range *get_ramsize(void)
{
	struct mem_range *mem = 0;
	if (!mem) {
		mem = sizeram();
	}
	if (!mem) {
		printk_emerg("No memory size information!\n");
		for(;;) {
			/* Ensure this loop is not optimized away */
			asm volatile("":/* outputs */:/*inputs */ :"memory");
		}
	}
	return mem;
}


#if CONFIG_SMP == 1
/* Number of cpus that are currently running in linuxbios */
static atomic_t active_cpus = ATOMIC_INIT(1);

void secondary_cpu_init(void)
{
	struct mem_range *mem;
	unsigned long id;
	int index;

	atomic_inc(&active_cpus);
	printk_debug(__FUNCTION__ "\n");
	mem = get_ramsize();
	id = cpu_initialize(mem);
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
			printk_err("CPU %d did not initialize!\n", i);
			processor_map[i] = 0;
#warning "FIXME do I need a mainboard_cpu_fixup function?"
		}
	}
	printk_debug("All AP CPUs stopped\n");
}

#else /* CONIFG_SMP */
#define wait_for_other_cpus() do {} while(0)
#endif /* CONFIG_SMP */

void hardwaremain(int boot_complete)
{
	/* Processor ID of the BOOT cpu (i.e. the one running this code) */
	unsigned long boot_cpu;
	int boot_index;

	/* the order here is a bit tricky. We don't want to do much of 
	 * anything that uses config registers until after PciAllocateResources
	 * since that function also figures out what kind of config strategy
	 * to use (type 1 or type 2). 
	 * so we turn on cache, then worry about PCI setup, then do other 
	 * things, so that the other work can use the PciRead* and PciWrite*
	 * functions. 
	 */
	struct mem_range *mem, *tmem;
	struct lb_memory *lb_mem;
	unsigned long totalmem;

	post_code(0x80);
	
	CONFIGURE(CONF_PASS_PRE_CONSOLE);

	/* displayinit MUST PRECEDE ALL PRINTK! */
	console_init();
	
	post_code(0x39);
	printk_notice("LinuxBIOS-%s%s %s %s...\n", 
		linuxbios_version, linuxbios_extra_version, linuxbios_build,
		(boot_complete)?"rebooting":"booting");

	post_code(0x40);

#if 0
	/* If we have already booted attempt a hard reboot */
	if (boot_complete) {
		hard_reset();
	}
#endif
	init_timer(); /* needs to be moved into static configuration */
	CONFIGURE(CONF_PASS_PRE_PCI);

	/* pick how to scan the bus. This is first so we can get at memory size. */
	printk_info("Finding PCI configuration type.\n");
	pci_set_method();
	post_code(0x5f);
#if 0
	enumerate_static_devices();
#endif
	dev_enumerate();
	post_code(0x66);
	/* Now do the real bus.
	 * We round the total ram up a lot for thing like the SISFB, which 
	 * shares high memory with the CPU. 
	 */
	dev_configure();
	post_code(0x88);

	dev_enable();

	dev_initialize();
	post_code(0x89);

	CONFIGURE(CONF_PASS_POST_PCI);

	mem = get_ramsize();
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

	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	lb_mem = write_tables(mem, processor_map);

	CONFIGURE(CONF_PASS_PRE_BOOT);

	elfboot(lb_mem);
}

