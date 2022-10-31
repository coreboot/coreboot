/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/hyperthreading.h>
#include <device/device.h>
#include <option.h>

/* Intel hyper-threading requires serialized CPU init. */

static int first_time = 1;
static int disable_siblings = !CONFIG(LOGICAL_CPUS);

void intel_sibling_init(struct device *cpu)
{
	unsigned int i, siblings;
	struct cpuid_result result;

	/* On the bootstrap processor see if I want sibling cpus enabled */
	if (first_time) {
		first_time = 0;
		disable_siblings = get_uint_option("hyper_threading", disable_siblings);
	}
	result = cpuid(1);
	/* Is hyperthreading supported */
	if (!(result.edx & (1 << 28)))
		return;

	/* See how many sibling cpus we have */
	siblings = (result.ebx >> 16) & 0xff;
	if (siblings < 1)
		siblings = 1;

	printk(BIOS_DEBUG, "CPU: %u %d siblings\n",
		cpu->path.apic.apic_id,
		siblings);

	/* See if I am a sibling cpu */
	if (cpu->path.apic.apic_id & (siblings - 1)) {
		if (disable_siblings)
			cpu->enabled = 0;
		return;
	}

	/* I am the primary CPU start up my siblings */
	for (i = 1; i < siblings; i++) {
		struct device_path cpu_path;
		struct device *new;
		/* Build the CPU device path */
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = cpu->path.apic.apic_id + i;

		/* Allocate new CPU device structure iff sibling CPU
		 * was not in static device tree.
		 */
		new = alloc_find_dev(cpu->bus, &cpu_path);

		if (!new)
			continue;

		printk(BIOS_DEBUG, "CPU: %u has sibling %u\n",
			cpu->path.apic.apic_id,
			new->path.apic.apic_id);
	}
}
