/*
 * This file is part of the coreboot project.
 *
 * Copyright 2012 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#define asmlinkage

#if !defined(__PRE_RAM__)
#include <device/device.h>

struct cpu_driver {
	struct device_operations *ops;
	struct cpu_device_id *id_table;
};

struct thread;

struct cpu_info {
	device_t cpu;
	unsigned long index;
#if CONFIG_COOP_MULTITASKING
	struct thread *thread;
#endif
};

#endif

struct cpu_info *cpu_info(void);

/*
 * Returns logical cpu in range [0:MAX_CPUS). SoC should define this.
 * Additionally, this is needed early in arm64 init so it should not
 * rely on a stack. Standard clobber list is fair game: x0-x7 and x0
 * returns the logical cpu number.
 */
unsigned int smp_processor_id(void);

/*
 * Do the necessary work to prepare for secondary CPUs coming up. The
 * SoC will call this function before bringing up the other CPUs. The
 * entry point for the seoncdary CPUs is returned.
 */
void *prepare_secondary_cpu_startup(void);

/*
 * Function provided by the SoC code that is called for each secondary
 * CPU startup.
 */
void soc_secondary_cpu_init(void);

#endif /* __ARCH_CPU_H__ */
