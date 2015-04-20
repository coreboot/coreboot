/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#define MSR_IA32_PLATFORM_ID		0x17
#define MSR_IA32_BIOS_SIGN_ID		0x8B
#define MSR_BSEL_CR_OVERCLOCK_CONTROL	0xcd
#define MSR_PLATFORM_INFO		0xce
#define MSR_PMG_CST_CONFIG_CONTROL	0xe2
#define		SINGLE_PCTL			(1 << 11)
#define MSR_POWER_MISC			0x120
#define		ENABLE_ULFM_AUTOCM_MASK		(1 << 2)
#define		ENABLE_INDP_AUTOCM_MASK		(1 << 3)
#define MSR_IA32_PERF_CTL		0x199
#define MSR_IA32_MISC_ENABLES		0x1a0
#define MSR_POWER_CTL			0x1fc
#define MSR_PKG_POWER_SKU_UNIT		0x606
#define MSR_PKG_POWER_LIMIT		0x610
#define MSR_PP1_POWER_LIMIT		0x638
#define MSR_IACORE_RATIOS		0x66a
#define MSR_IACORE_TURBO_RATIOS		0x66c
#define MSR_IACORE_VIDS			0x66b
#define MSR_IACORE_TURBO_VIDS		0x66d
#define MSR_PKG_TURBO_CFG1		0x670
#define MSR_CPU_TURBO_WKLD_CFG1		0x671
#define MSR_CPU_TURBO_WKLD_CFG2		0x672
#define MSR_CPU_THERM_CFG1		0x673
#define MSR_CPU_THERM_CFG2		0x674
#define MSR_CPU_THERM_SENS_CFG		0x675

#define BUS_FREQ_KHZ			100000	/* 100 MHz */

#endif /* _SOC_MSR_H_ */
