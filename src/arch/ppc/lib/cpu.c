#include <console/console.h>
#include <mem.h>
#include <arch/io.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/ppc/cpuid.h>
#include <smp/start_stop.h>

static void cache_on(struct mem_range *mem)
{
}

static void interrupts_on()
{
}

unsigned long cpu_initialize(struct mem_range *mem)
{
	/* Because we busy wait at the printk spinlock.
	 * It is important to keep the number of printed messages
	 * from secondary cpus to a minimum, when debugging is
	 * disabled.
	 */
	unsigned long processor_id = this_processors_id();
	printk_notice("Initializing CPU #%d\n", processor_id);
	
	/* Turn on caching if we haven't already */
	cache_on(mem);

	display_cpuid();

	interrupts_on();

	printk_info("CPU #%d Initialized\n", processor_id);
	return processor_id;
}

