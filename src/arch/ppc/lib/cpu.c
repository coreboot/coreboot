#include <console/console.h>
#include <arch/io.h>
#include <string.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/ppc/cpuid.h>
#include "ppc.h"
#include "ppcreg.h"

#if 0
static void set_cpu_ops(struct device *cpu)
{
	struct cpu_driver *driver;
	cpu->ops = 0;
	for (driver = cpu_drivers; driver < ecpu_drivers; driver++) {
		struct cpu_device_id *id;
		for(id = driver->id_table; id->pvr != 0; id++) {
			if (cpu->device == id->pvr) 
			{
				goto found;
			}
		}
	}
	die("Unknown cpu");
	return;
 found:
	cpu->ops = driver->ops;
}
#endif

void cpu_initialize(void)
{
	/* Because we busy wait at the printk spinlock.
	 * It is important to keep the number of printed messages
	 * from secondary cpus to a minimum, when debugging is
	 * disabled.
	 */
	struct device *cpu;
	struct cpu_info *info;
	info = cpu_info();

	printk_notice("Initializing CPU #%d\n", info->index);

	cpu = info->cpu;
	if (!cpu) {
		die("CPU: missing cpu device structure");
	}
	
	/* Find what type of cpu we are dealing with */
	cpu->vendor = 0; /* PPC cpus do not have a vendor field */
	cpu->device = ppc_getpvr();
	display_cpuid(cpu);

#if 0
	/* Lookup the cpu's operations */
	set_cpu_ops(cpu);

	/* Initialize the cpu */
	if (cpu->ops && cpu->ops->init) {
		cpu->enabled = 1;
		cpu->initialized = 1;
		cpu->ops->init();
	}
#endif	
	/* Turn on caching if we haven't already */

	printk_info("CPU #%d initialized\n", info->index);
	return;
}
