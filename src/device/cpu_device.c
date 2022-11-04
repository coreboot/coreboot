/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <stddef.h>

struct device *add_cpu_device(struct bus *cpu_bus, unsigned int apic_id,
			      int enabled)
{
	struct device_path cpu_path = {};
	struct device *cpu;

	/* Build the CPU device path */
	cpu_path.type = DEVICE_PATH_APIC;
	cpu_path.apic.apic_id = apic_id;
	cpu_path.apic.initial_lapicid = apic_id;

	/* Update CPU in devicetree. */
	if (enabled)
		cpu = alloc_find_dev(cpu_bus, &cpu_path);
	else
		cpu = find_dev_path(cpu_bus, &cpu_path);
	if (!cpu)
		return NULL;

	cpu->enabled = enabled;
	printk(BIOS_DEBUG, "CPU: %s %s\n",
		dev_path(cpu), cpu->enabled?"enabled":"disabled");

	return cpu;
}
