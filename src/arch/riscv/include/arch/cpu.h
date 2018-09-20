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
 */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#include <arch/encoding.h>

#define asmlinkage

#if !defined(__PRE_RAM__)
#include <device/device.h>

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct thread;

struct cpu_info {
	struct device *cpu;
	unsigned long index;
#if IS_ENABLED(CONFIG_COOP_MULTITASKING)
	struct thread *thread;
#endif
};

struct cpuinfo_riscv {
	uint8_t    riscv;            /* CPU family */
	uint8_t    riscv_vendor;     /* CPU vendor */
	uint8_t    riscv_model;
};

#endif

static inline int supports_extension(char ext)
{
	return read_csr(misa) & (1 << (ext - 'A'));
}

static inline int machine_xlen(void)
{
	int mxl = (read_csr(misa) >> (__riscv_xlen - 2)) & 3;
	return (1 << mxl) * 16;
}

struct cpu_info *cpu_info(void);
#endif /* __ARCH_CPU_H__ */
