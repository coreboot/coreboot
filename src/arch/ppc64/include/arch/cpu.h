/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#include <device/device.h>

#define asmlinkage

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct thread;

struct cpu_info {
	struct device *cpu;
	unsigned long index;
#if CONFIG(COOP_MULTITASKING)
	struct thread *thread;
#endif
};

struct cpuinfo_ppc64 {
	uint8_t    ppc64;            /* CPU family */
	uint8_t    ppc64_vendor;     /* CPU vendor */
	uint8_t    ppc64_model;
};

struct cpu_info *cpu_info(void);

#endif /* __ARCH_CPU_H__ */
