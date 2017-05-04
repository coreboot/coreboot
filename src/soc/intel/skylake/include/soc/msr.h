/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <intelblocks/msr.h>

#define MSR_PIC_MSG_CONTROL		0x2e
#define MSR_BIOS_UPGD_TRIG		0x7a
#define IA32_THERM_INTERRUPT		0x19b
#define IA32_ENERGY_PERFORMANCE_BIAS	0x1b0
#define  ENERGY_POLICY_PERFORMANCE	0
#define  ENERGY_POLICY_NORMAL		6
#define  ENERGY_POLICY_POWERSAVE	15
#define IA32_PACKAGE_THERM_INTERRUPT	0x1b2
#define PRMRR_PHYS_BASE_MSR		0x1f4
#define IA32_PLATFORM_DCA_CAP		0x1f8
#define MSR_LT_LOCK_MEMORY		0x2e7
#define MSR_SGX_OWNEREPOCH0		0x300
#define MSR_SGX_OWNEREPOCH1		0x301
#define MSR_VR_CURRENT_CONFIG		0x601
#define MSR_VR_MISC_CONFIG		0x603
#define MSR_VR_MISC_CONFIG2		0x636
#define MSR_PP0_POWER_LIMIT		0x638
#define MSR_PP1_POWER_LIMIT		0x640

#endif
