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

#ifndef __ASSEMBLER__
#include <cpu/x86/msr.h>
#include <device/device.h>

void apollolake_init_cpus(struct device *dev);
void set_max_freq(void);
#endif

#define CPUID_APOLLOLAKE_A0	0x506c8
#define CPUID_APOLLOLAKE_B0	0x506c9

#define MSR_PLATFORM_INFO	0xce
#define MSR_POWER_MISC		0x120
#define   ENABLE_IA_UNTRUSTED	(1 << 6)
#define   FLUSH_DL1_L2		(1 << 8)
#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_EVICT_CTL		0x2e0
#define MSR_EMULATE_PM_TMR	0x121
#define   EMULATE_PM_TMR_EN	(1 << 16)
#define MSR_PREFETCH_CTL	0x1a4
#define   PREFETCH_L1_DISABLE	(1 << 0)
#define   PREFETCH_L2_DISABLE	(1 << 2)

#define MSR_PKG_POWER_SKU_UNIT	0x606
#define MSR_PKG_POWER_SKU	0x614
#define MSR_PKG_POWER_LIMIT	0x610
#define PKG_POWER_LIMIT_MASK		(0x7fff)
#define PKG_POWER_LIMIT_EN		(1 << 15)
#define PKG_POWER_LIMIT_CLAMP		(1 << 16)
#define PKG_POWER_LIMIT_TIME_SHIFT	17
#define PKG_POWER_LIMIT_TIME_MASK	(0x7f)
/*
 * For Mobile, RAPL default PL1 time window value set to 28 seconds.
 * RAPL time window calculation defined as follows:
 * Time Window = (float)((1+X/4)*(2*^Y), X Corresponds to [23:22],
 * Y to [21:17] in MSR 0x610. 28 sec is equal to 0x6e.
 */
#define MB_POWER_LIMIT1_TIME_DEFAULT	0x6e

/* Set MSR_PMG_CST_CONFIG_CONTROL[3:0] for Package C-State limit */
#define PKG_C_STATE_LIMIT_C2_MASK	0x2
/* Set MSR_PMG_CST_CONFIG_CONTROL[7:4] for Core C-State limit*/
#define CORE_C_STATE_LIMIT_C10_MASK	0x70
/* Set MSR_PMG_CST_CONFIG_CONTROL[10] to IO redirect to MWAIT */
#define IO_MWAIT_REDIRECT_MASK	0x400
/* Set MSR_PMG_CST_CONFIG_CONTROL[15] to lock CST_CFG [0-15] bits */
#define CST_CFG_LOCK_MASK	0x8000

#define MSR_PMG_CST_CONFIG_CONTROL	0xe2
#define MSR_PMG_IO_CAPTURE_BASE	0xe4
#define MSR_FEATURE_CONFIG	0x13c
#define FEATURE_CONFIG_RESERVED_MASK	0x3ULL
#define FEATURE_CONFIG_LOCK	(1 << 0)
#define MSR_POWER_CTL	0x1fc

#define MSR_L2_QOS_MASK(reg)		(0xd10 + reg)
#define MSR_IA32_PQR_ASSOC		0xc8f
/* MSR bits 33:32 encode slot number 0-3 */
#define   IA32_PQR_ASSOC_MASK		(1 << 0 | 1 << 1)
/* 16 way cache, 8 bits per QOS, 64 byte cache line, 1024 sets */
#define CACHE_WAYS			16
#define CACHE_BITS_PER_MASK		8
#define CACHE_LINE_SIZE			64
#define CACHE_SETS			1024
/*
 * Each bit in QOS mask controls this many bytes. This is calculated as:
 * (CACHE_WAYS / CACHE_BITS_PER_MASK) * CACHE_LINE_SIZE * CACHE_SETS
 */
#define CACHE_QOS_SIZE_PER_BIT		(128 * KiB)
#define L2_CACHE_SIZE			0x100000

#define BASE_CLOCK_MHZ		100

/* Common Timer Copy (CTC) frequency - 19.2MHz. */
#define CTC_FREQ 19200000

/* This is burst mode BIT 38 in MSR_IA32_MISC_ENABLES MSR at offset 1A0h */
#define APL_BURST_MODE_DISABLE		0x40

#endif /* _SOC_APOLLOLAKE_CPU_H_ */
