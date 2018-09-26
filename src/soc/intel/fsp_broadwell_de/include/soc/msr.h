/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#define MSR_IA32_PLATFORM_ID	0x17
#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_PLATFORM_INFO	0xce
#define IA32_MCG_CAP		0x179
#define IA32_PERF_CTL		0x199
#define MSR_TURBO_RATIO_LIMIT	0x1ad
#define IA32_MC0_CTL		0x400
#define IA32_MC0_STATUS		0x401
#define MSR_PKG_POWER_SKU_UNIT	0x606
#define MSR_PKG_POWER_LIMIT	0x610
#define MSR_CONFIG_TDP_NOMINAL	0x648

#define SMM_MCA_CAP_MSR		0x17d
#define  SMM_CPU_SVRSTR_BIT	57
#define  SMM_CPU_SVRSTR_MASK	(1 << (SMM_CPU_SVRSTR_BIT - 32))

/* SMM save state MSRs */
#define SMBASE_MSR		0xc20
#define IEDBASE_MSR		0xc22
/* MTRR_CAP_MSR bits */
#define SMRR_SUPPORTED		(1 << 11)
#define PRMRR_SUPPORTED		(1 << 12)
#define PRMRRphysBase_MSR	0x1f4
#define PRMRRphysMask_MSR	0x1f5

#endif /* _SOC_MSR_H_ */
