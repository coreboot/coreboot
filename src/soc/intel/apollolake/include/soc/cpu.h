/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _SOC_APOLLOLAKE_CPU_H_
#define _SOC_APOLLOLAKE_CPU_H_

#ifndef __ASSEMBLER__
#include <cpu/x86/msr.h>
#include <device/device.h>

void apollolake_init_cpus(struct device *dev);
#endif

#define CPUID_APOLLOLAKE_A0	0x506c8
#define CPUID_APOLLOLAKE_B0	0x506c9

#define MSR_PLATFORM_INFO	0xce
#define MSR_POWER_MISC		0x120
#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_EVICT_CTL		0x2e0

#define BASE_CLOCK_MHZ		100

#endif /* _SOC_APOLLOLAKE_CPU_H_ */
