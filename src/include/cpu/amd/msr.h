/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#ifndef CPU_AMD_MSR_H
#define CPU_AMD_MSR_H

#include <cpu/x86/msr.h>

#define SMM_BASE_MSR			0xC0010111
#define SMM_ADDR_MSR			0xC0010112
#define SMM_MASK_MSR			0xC0010113

#define HWCR_MSR			0xC0010015
#define NB_CFG_MSR			0xC001001f
#define LS_CFG_MSR			0xC0011020
#define IC_CFG_MSR			0xC0011021
#define DC_CFG_MSR			0xC0011022
#define BU_CFG_MSR			0xC0011023
#define FP_CFG_MSR			0xC0011028
#define DE_CFG_MSR			0xC0011029
#define BU_CFG2_MSR			0xC001102A
#define BU_CFG3_MSR			0xC001102B
#define EX_CFG_MSR			0xC001102C
#define LS_CFG2_MSR			0xC001102D
#define IBS_OP_DATA3_MSR		0xC0011037

#define CPU_ID_FEATURES_MSR		0xC0011004
#define CPU_ID_HYPER_EXT_FEATURES	0xC001100d
#define LOGICAL_CPUS_NUM_MSR		0xC001100d
#define CPU_ID_EXT_FEATURES_MSR		0xC0011005

#endif /* CPU_AMD_MSR_H */
