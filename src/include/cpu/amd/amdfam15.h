/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef CPU_AMD_FAM15_H
#define CPU_AMD_FAM15_H

#define MC0_STATUS			0x00000401
#define MSR_SMM_BASE			0xC0010111
#define MSR_TSEG_BASE			0xC0010112
#define MSR_SMM_MASK			0xC0010113
# define SMM_TSEG_VALID			(1 << 1)
# define SMM_TSEG_WB			(6 << 12)
#define HWCR_MSR			0xC0010015
# define SMM_LOCK			(1 << 0)
#define NB_CFG_MSR			0xC001001f

#define MMIO_CONF_BASE			0xC0010058
# define MMIO_BUS_RANGE_SHIFT		2
# define MMIO_RANGE_EN			(1 << 0)

#define PSTATE_0_MSR			0xC0010064

#define LS_CFG_MSR			0xC0011020
#define IC_CFG_MSR			0xC0011021
#define DC_CFG_MSR			0xC0011022
#define CU_CFG_MSR			0xC0011023
#define CU_CFG2_MSR			0xC001102A

#define CPU_ID_FEATURES_MSR		0xC0011004
#define CPU_ID_EXT_FEATURES_MSR		0xC0011005

#define CORE_PERF_BOOST_CTRL		0x15C

#endif /* CPU_AMD_FAM15_H */
