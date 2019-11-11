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

#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_PLATFORM_INFO	0xce
#define MSR_TURBO_RATIO_LIMIT	0x1ad
#define MSR_PKG_POWER_SKU_UNIT	0x606
#define MSR_PKG_POWER_LIMIT	0x610
#define MSR_UNCORE_RATIO_LIMIT	0x620
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
#define MSR_PRMRR_PHYS_BASE	0x1f4
#define MSR_PRMRR_PHYS_MASK	0x1f5

/* EDS vol 2 */
#define MSR_LT_MEMORY_LOCKED	0x2e7
#define   MSR_MEM_LOCK_BIT1	(1 << 1)
#define   MSR_MEM_LOCK_BIT2	(1 << 2)

#endif /* _SOC_MSR_H_ */
