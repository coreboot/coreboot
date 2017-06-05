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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_CPU_H_
#define _SOC_APOLLOLAKE_CPU_H_

#include <intelblocks/msr.h>

#ifndef __ASSEMBLER__
#include <cpu/x86/msr.h>
#include <device/device.h>

void set_max_freq(void);
void enable_untrusted_mode(void);
#endif

#define CPUID_APOLLOLAKE_A0	0x506c8
#define CPUID_APOLLOLAKE_B0	0x506c9

/* Common Timer Copy (CTC) frequency - 19.2MHz. */
#define CTC_FREQ		19200000

/* This is burst mode BIT 38 in MSR_IA32_MISC_ENABLES MSR at offset 1A0h */
#define APL_BURST_MODE_DISABLE		0x40

#endif /* _SOC_APOLLOLAKE_CPU_H_ */
