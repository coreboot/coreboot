/*
 * This file is part of the coreboot project.
 *
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

#ifndef SOC_INTEL_COMMON_BLOCK_MP_INIT_H
#define SOC_INTEL_COMMON_BLOCK_MP_INIT_H

#include <device/device.h>

/* Supported CPUIDs for different SOCs */
#define CPUID_SKYLAKE_C0	0x406e2
#define CPUID_SKYLAKE_D0	0x406e3
#define CPUID_SKYLAKE_HQ0	0x506e1
#define CPUID_SKYLAKE_HR0	0x506e3
#define CPUID_KABYLAKE_G0	0x406e8
#define CPUID_KABYLAKE_H0	0x806e9
#define CPUID_KABYLAKE_Y0	0x806ea
#define CPUID_KABYLAKE_HA0	0x506e8
#define CPUID_KABYLAKE_HB0	0x906e9
#define CPUID_CANNONLAKE_A0	0x60660
#define CPUID_CANNONLAKE_B0	0x60661
#define CPUID_CANNONLAKE_C0	0x60662
#define CPUID_CANNONLAKE_D0	0x60663
#define CPUID_APOLLOLAKE_A0	0x506c8
#define CPUID_APOLLOLAKE_B0	0x506c9
#define CPUID_APOLLOLAKE_E0	0x506ca
#define CPUID_GLK_A0		0x706a0
#define CPUID_GLK_B0		0x706a1
#define CPUID_WHISKEYLAKE_W0	0x806eb
#define CPUID_COFFEELAKE_D0	0x806ea
#define CPUID_COFFEELAKE_U0	0x906ea

/*
 * MP Init callback function to Find CPU Topology. This function is common
 * among all SOCs and thus its in Common CPU block.
 */
int get_cpu_count(void);

/*
 * Function to get the microcode patch pointer. Use this function to avoid
 * reading the microcode patch from the boot media. init_cpus() would
 * initialize microcode_patch global variable to point to microcode patch
 * in boot media and this function can be used to access the pointer.
 */
const void *intel_mp_current_microcode(void);

/*
 * MP Init callback function(get_microcode_info) to find the Microcode at
 * Pre MP Init phase. This function is common among all SOCs and thus its in
 * Common CPU block.
 * This function also fills in the microcode patch (in *microcode), and also
 * sets the argument *parallel to 1, which allows microcode loading in all
 * APs to occur in parallel during MP Init.
 */
void get_microcode_info(const void **microcode, int *parallel);

/*
 * SoC Overrides
 *
 * All new SoC must implement below functionality for ramstage.
 */

/*
 * In this function SOC must perform CPU feature programming
 * during Ramstage phase.
 */
void soc_core_init(struct device *dev);

/*
 * In this function SOC must fill required mp_ops params, also it
 * should call these mp_ops callback functions by calling
 * mp_init_with_smm() function from x86/mp_init.c file.
 *
 * Also, if there is any other SOC specific functionalities to be
 * implemented before or after MP Init, it can be done here.
 */
void soc_init_cpus(struct bus *cpu_bus);

#endif	/* SOC_INTEL_COMMON_BLOCK_MP_INIT_H */
