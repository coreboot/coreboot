/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CPU_AMD_FAM14_H
#define CPU_AMD_FAM14_H

#include <cpu/x86/msr.h>

#define HWCR_MSR			0xC0010015
#define NB_CFG_MSR			0xC001001f
#define LS_CFG_MSR			0xC0011020
#define IC_CFG_MSR			0xC0011021
#define DC_CFG_MSR			0xC0011022
#define BU_CFG_MSR			0xC0011023
#define BU_CFG2_MSR			0xC001102A

#define CPU_ID_FEATURES_MSR		0xC0011004
#define CPU_ID_EXT_FEATURES_MSR		0xC0011005

#if defined(__PRE_RAM__)
void wait_all_core0_started(void);
void wait_all_other_cores_started(u32 bsp_apicid);
void wait_all_aps_started(u32 bsp_apicid);
void allow_all_aps_stop(u32 bsp_apicid);
#endif
void get_bus_conf(void);
u32 get_initial_apicid(void);

#endif /* CPU_AMD_FAM14_H */
