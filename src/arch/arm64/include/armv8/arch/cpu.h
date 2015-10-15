/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#define asmlinkage

#if !defined(__PRE_RAM__)
#include <arch/barrier.h>
#include <arch/mpidr.h>
#include <device/device.h>

enum {
	CPU_ID_END = 0x00000000,
};

struct cpu_device_id {
	uint32_t midr;
};

struct cpu_driver {
	/* This is excessive as init() is the only one called. */
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct cpu_info {
	device_t cpu;
	unsigned int id;
	uint64_t mpidr;
};

/* Initialize CPU0 under the DEVICE_PATH_CPU_CLUSTER cluster. */
void arch_initialize_cpu(device_t cluster);

#endif /* !__PRE_RAM__ */

static inline unsigned int smp_processor_id(void) { return 0; }

/*
 * The arm64_cpu_startup() initializes CPU's exception stack and regular
 * stack as well initializing the C environment for the processor. Finally it
 * calls into c_entry.
 */
void arm64_cpu_startup(void);

/*
 * The arm64_arch_timer_init() initializes the CPU's cntfrq register of
 * ARM arch timer.
 */
void arm64_arch_timer_init(void);

#endif /* __ARCH_CPU_H__ */
