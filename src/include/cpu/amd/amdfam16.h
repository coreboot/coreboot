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

#ifndef CPU_AMD_FAM16_H
#define CPU_AMD_FAM16_H

#define MC0_STATUS			0x00000401
#define HWCR_MSR			0xC0010015
#define NB_CFG_MSR			0xC001001f

#define LS_CFG_MSR			0xC0011020
#define IC_CFG_MSR			0xC0011021
#define DC_CFG_MSR			0xC0011022
#define CU_CFG_MSR			0xC0011023
#define CU_CFG2_MSR			0xC001102A

#define CPU_ID_FEATURES_MSR		0xC0011004
#define CPU_ID_EXT_FEATURES_MSR		0xC0011005

#endif /* CPU_AMD_FAM16_H */
