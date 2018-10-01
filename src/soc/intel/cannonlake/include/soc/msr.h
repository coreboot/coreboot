/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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
#define IA32_THERM_INTERRUPT		0x19b
#define IA32_ENERGY_PERFORMANCE_BIAS	0x1b0
#define  ENERGY_POLICY_PERFORMANCE	0
#define  ENERGY_POLICY_NORMAL		6
#define  ENERGY_POLICY_POWERSAVE	15
#define IA32_PACKAGE_THERM_INTERRUPT	0x1b2
#define IA32_PLATFORM_DCA_CAP		0x1f8
#define MSR_VR_MISC_CONFIG2		0x636

#endif
