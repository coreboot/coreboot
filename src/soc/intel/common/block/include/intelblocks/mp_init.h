/* SPDX-License-Identifier: GPL-2.0-only */

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
#define CPUID_GLK_R0		0x706a8
#define CPUID_WHISKEYLAKE_V0	0x806ec
#define CPUID_WHISKEYLAKE_W0	0x806eb
#define CPUID_COFFEELAKE_U0	0x906ea
#define CPUID_COFFEELAKE_B0	0x906eb
#define CPUID_COFFEELAKE_P0	0x906ec
#define CPUID_COFFEELAKE_R0	0x906ed
#define CPUID_ICELAKE_A0	0x706e0
#define CPUID_ICELAKE_B0	0x706e1
#define CPUID_JASPERLAKE_A0	0x906c0
#define CPUID_COMETLAKE_U_A0		0xa0660
#define CPUID_COMETLAKE_U_K0_S0		0xa0661
#define CPUID_COMETLAKE_H_S_6_2_G0	0xa0650
#define CPUID_COMETLAKE_H_S_6_2_G1	0xa0653
#define CPUID_COMETLAKE_H_S_10_2_P0	0xa0651
#define CPUID_COMETLAKE_H_S_10_2_Q0_P1	0xa0654
#define CPUID_TIGERLAKE_A0	0x806c0
#define CPUID_TIGERLAKE_B0	0x806c1
#define CPUID_ELKHARTLAKE_A0	0x90660
#define CPUID_ELKHARTLAKE_B0	0x90661
#define CPUID_ALDERLAKE_S_A0	0x90670
#define CPUID_ALDERLAKE_P_A0	0x906a0
#define CPUID_ALDERLAKE_M_A0	0x906a1
/*
 * MP Init callback function to Find CPU Topology. This function is common
 * among all SOCs and thus its in Common CPU block.
 */
int get_cpu_count(void);

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
 * Perform BSP and AP initialization
 * This function can be called in below cases
 * 1. During coreboot is doing MP initialization as part of BS_DEV_INIT_CHIPS (exclude
 * this call if user has selected USE_INTEL_FSP_MP_INIT)
 * 2. coreboot would like to take APs control back after FSP-S has done with MP
 * initialization based on user select USE_INTEL_FSP_MP_INIT
 */
void init_cpus(void);

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
