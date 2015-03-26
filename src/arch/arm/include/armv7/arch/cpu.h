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
 * Foundation, Inc.
 */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#include <stdint.h>

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

struct cpuinfo_arm {
        uint8_t    arm;            /* CPU family */
        uint8_t    arm_vendor;     /* CPU vendor */
        uint8_t    arm_model;
};

#endif

/* Primitives for CPU and MP cores. */

/* read Main Id register (MIDR) */
inline static uint32_t read_midr(void)
{
	uint32_t value;
	asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r"(value));
	return value;
}

/* read Multiprocessor Affinity Register (MPIDR) */
inline static uint32_t read_mpidr(void)
{
	uint32_t value;
	asm volatile ("mrc p15, 0, %0, c0, c0, 5" : "=r"(value));
	return value;
}

/* read Auxiliary Control Register (ACTLR) */
inline static uint32_t read_actlr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 0, %0, c1, c0, 1" : "=r"(val));
	return val;
}

/* write Auxiliary Control Register (ACTLR) */
inline static void write_actlr(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c1, c0, 1" : : "r" (val));
}

/* wait for interrupt. */
inline static void wfi(void)
{
	asm volatile ("wfi" : : : "memory");
}

/* wait for event. */
inline static void wfe(void)
{
	asm volatile ("wfe");
}

/* set event (to bring up cores in WFE state). */
inline static void sev(void)
{
	asm volatile ("sev");
}

/* puts CPU into System mode and disable interrupts. */
inline static void set_system_mode(void)
{
	asm volatile("msr cpsr_c, %0" :: "r"(0x1f | 0xc0));
}

struct cpu_info *cpu_info(void);
#endif /* __ARCH_CPU_H__ */
