/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_CPU_H_
#define _SOC_CPU_H_

#include <device/device.h>
#include <cpu/x86/msr.h>

/* SKXSP CPUID */
#define CPUID_SKYLAKE_SP_A0_A1 0x506f0
#define CPUID_SKYLAKE_SP_B0 0x506f1
#define CPUID_SKYLAKE_SP_4 0x50654

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK 100

int get_cpu_count(void);
void xeon_sp_init_cpus(struct device *dev);

#endif
