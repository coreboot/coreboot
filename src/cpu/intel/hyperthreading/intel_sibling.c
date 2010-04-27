#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/hyperthreading.h>
#include <device/device.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>

static int first_time = 1;
static int disable_siblings = !CONFIG_LOGICAL_CPUS;

void intel_sibling_init(device_t cpu)
{
	unsigned i, siblings;
	struct cpuid_result result;

	/* On the bootstrap processor see if I want sibling cpus enabled */
	if (first_time) {
		first_time = 0;
		get_option(&disable_siblings, "hyper_threading");
	}
	result = cpuid(1);
	/* Is hyperthreading supported */
	if (!(result.edx & (1 << 28))) {
		return;
	}
	/* See how many sibling cpus we have */
	siblings = (result.ebx >> 16) & 0xff;
	if (siblings < 1) {
		siblings = 1;
	}

#if 1
	printk(BIOS_DEBUG, "CPU: %u %d siblings\n",
		cpu->path.apic.apic_id,
		siblings);
#endif

	/* See if I am a sibling cpu */
	if (cpu->path.apic.apic_id & (siblings -1)) {
		if (disable_siblings) {
			cpu->enabled = 0;
		}
		return;
	}

	/* I am the primary cpu start up my siblings */
	for(i = 1; i < siblings; i++) {
		struct device_path cpu_path;
		device_t new;
		/* Build the cpu device path */
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = cpu->path.apic.apic_id + i;


		/* Allocate the new cpu device structure */
		new = alloc_dev(cpu->bus, &cpu_path);

		if (!new) {
			continue;
		}

#if 1
		printk(BIOS_DEBUG, "CPU: %u has sibling %u\n",
			cpu->path.apic.apic_id,
			new->path.apic.apic_id);
#endif
		/* Start the new cpu */
		if (!start_cpu(new)) {
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU %u would not start!\n",
				new->path.apic.apic_id);
		}
	}

}

