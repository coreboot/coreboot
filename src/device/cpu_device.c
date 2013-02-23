/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2012 Kyösti Mälkki <kyosti.malkki@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/device.h>
#include <console/console.h>
#include <cpu/x86/lapic.h>

void remap_bsp_lapic(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	device_t cpu;
	u32 bsp_lapic_id = lapicid();

	if (bsp_lapic_id) {
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = 0;
		cpu = find_dev_path(cpu_bus, &cpu_path);
		if (cpu)
			cpu->path.apic.apic_id = bsp_lapic_id;
	}
}

device_t add_cpu_device(struct bus *cpu_bus, unsigned apic_id, int enabled)
{
	struct device_path cpu_path;
	device_t cpu;

	/* Build the cpu device path */
	cpu_path.type = DEVICE_PATH_APIC;
	cpu_path.apic.apic_id = apic_id;

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

void set_cpu_topology(device_t cpu, unsigned node, unsigned package, unsigned core, unsigned thread)
{
	cpu->path.apic.node_id = node;
	cpu->path.apic.package_id = package;
	cpu->path.apic.core_id = core;
	cpu->path.apic.thread_id = thread;
}

