/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CPU_H
#define _SOC_CPU_H

#include <device/device.h>
#include <cpu/x86/msr.h>

#define CPUID_COOPERLAKE_SP_A0			0x05065a
#define CPUID_COOPERLAKE_SP_A1			0x05065b

void cpx_init_cpus(struct device *dev);

#endif
