/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_CPU_H_
#define _SOC_CPU_H_

#include <device/device.h>

/* SKXSP CPUID */
#define CPUID_SKYLAKE_SP_A0_A1 0x506f0
#define CPUID_SKYLAKE_SP_B0 0x506f1
#define CPUID_SKYLAKE_SP_4 0x50654

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK 100

int get_cpu_count(void);
void xeon_sp_init_cpus(struct device *dev);

#endif
