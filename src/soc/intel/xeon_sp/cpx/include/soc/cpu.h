/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CPU_H
#define _SOC_CPU_H

#include <device/device.h>
#include <cpu/x86/msr.h>

#define CPUID_COOPERLAKE_SP_A0			0x05065a

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK 100

void cpx_init_cpus(struct device *dev);
msr_t read_msr_ppin(void);

#endif
