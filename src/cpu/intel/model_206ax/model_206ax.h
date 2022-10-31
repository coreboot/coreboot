/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_MODEL_206AX_H
#define _CPU_INTEL_MODEL_206AX_H

#include <cpu/cpu.h>
#include <stdint.h>

/* SandyBridge CPU stepping */
#define SNB_STEP_B2	2
#define SNB_STEP_C0	3
#define SNB_STEP_D0	5 /* Also J0 */
#define SNB_STEP_D1	6
#define SNB_STEP_D2	7 /* Also J1/Q0 */

/* IvyBridge CPU stepping */
#define IVB_STEP_A0	0
#define IVB_STEP_B0	2
#define IVB_STEP_C0	4
#define IVB_STEP_K0	5
#define IVB_STEP_D0	6
#define IVB_STEP_E0	8
#define IVB_STEP_E1	9

#define IS_SANDY_CPU(x)    ((x & 0xffff0) == 0x206a0)
#define IS_SANDY_CPU_C(x)  ((x & 0xf) == 4)
#define IS_SANDY_CPU_D0(x) ((x & 0xf) == 5)
#define IS_SANDY_CPU_D1(x) ((x & 0xf) == 6)
#define IS_SANDY_CPU_D2(x) ((x & 0xf) == 7)

#define IS_IVY_CPU(x)   ((x & 0xffff0) == 0x306a0)
#define IS_IVY_CPU_C(x) ((x & 0xf) == 4)
#define IS_IVY_CPU_K(x) ((x & 0xf) == 5)
#define IS_IVY_CPU_D(x) ((x & 0xf) == 6)
#define IS_IVY_CPU_E(x) ((x & 0xf) >= 8)

/* SandyBridge/IvyBridge bus clock is fixed at 100MHz */
#define SANDYBRIDGE_BCLK		100

#define MSR_CORE_THREAD_COUNT		0x35
#define MSR_FEATURE_CONFIG		0x13c
#define MSR_FLEX_RATIO			0x194
#define  FLEX_RATIO_LOCK		(1 << 20)
#define  FLEX_RATIO_EN			(1 << 16)
#define MSR_TEMPERATURE_TARGET		0x1a2
#define MSR_LT_LOCK_MEMORY		0x2e7
#define MSR_PLATFORM_INFO		0xce
#define  PLATFORM_INFO_SET_TDP		(1 << 29)

#define MSR_MISC_PWR_MGMT		0x1aa
#define  MISC_PWR_MGMT_EIST_HW_DIS	(1 << 0)
#define MSR_TURBO_RATIO_LIMIT		0x1ad
#define MSR_POWER_CTL			0x1fc

#define MSR_PKGC3_IRTL			0x60a
#define MSR_PKGC6_IRTL			0x60b
#define MSR_PKGC7_IRTL			0x60c
#define  IRTL_VALID			(1 << 15)
#define  IRTL_1_NS			(0 << 10)
#define  IRTL_32_NS			(1 << 10)
#define  IRTL_1024_NS			(2 << 10)
#define  IRTL_32768_NS			(3 << 10)
#define  IRTL_1048576_NS		(4 << 10)
#define  IRTL_33554432_NS		(5 << 10)
#define  IRTL_RESPONSE_MASK		(0x3ff)

/* long duration in low dword, short duration in high dword */
#define MSR_PKG_POWER_LIMIT		0x610
#define  PKG_POWER_LIMIT_MASK		0x7fff
#define  PKG_POWER_LIMIT_EN		(1 << 15)
#define  PKG_POWER_LIMIT_CLAMP		(1 << 16)
#define  PKG_POWER_LIMIT_TIME_SHIFT	17
#define  PKG_POWER_LIMIT_TIME_MASK	0x7f

#define MSR_PP0_CURRENT_CONFIG		0x601
#define  PP0_CURRENT_LIMIT		(112 << 3) /* 112 A */
#define MSR_PP1_CURRENT_CONFIG		0x602
#define  PP1_CURRENT_LIMIT_SNB		(35 << 3) /* 35 A */
#define  PP1_CURRENT_LIMIT_IVB		(50 << 3) /* 50 A */
#define MSR_PKG_POWER_SKU_UNIT		0x606
#define MSR_PKG_POWER_SKU		0x614
#define MSR_PP0_POWER_LIMIT		0x638
#define MSR_PP1_POWER_LIMIT		0x640

#define IVB_CONFIG_TDP_MIN_CPUID	0x306a2
#define MSR_CONFIG_TDP_NOMINAL		0x648
#define MSR_CONFIG_TDP_LEVEL1		0x649
#define MSR_CONFIG_TDP_LEVEL2		0x64a
#define MSR_CONFIG_TDP_CONTROL		0x64b
#define MSR_TURBO_ACTIVATION_RATIO	0x64c

/* P-state configuration */
#define PSS_MAX_ENTRIES			8
#define PSS_RATIO_STEP			2
#define PSS_LATENCY_TRANSITION		10
#define PSS_LATENCY_BUSMASTER		10

/* Sanity check config options. */
#if (CONFIG_SMM_TSEG_SIZE <= (CONFIG_IED_REGION_SIZE + CONFIG_SMM_RESERVED_SIZE))
# error "CONFIG_SMM_TSEG_SIZE <= (CONFIG_IED_REGION_SIZE + CONFIG_SMM_RESERVED_SIZE)"
#endif
#if (CONFIG_SMM_TSEG_SIZE < 0x800000)
# error "CONFIG_SMM_TSEG_SIZE must at least be 8MiB"
#endif
#if ((CONFIG_SMM_TSEG_SIZE & (CONFIG_SMM_TSEG_SIZE - 1)) != 0)
# error "CONFIG_SMM_TSEG_SIZE is not a power of 2"
#endif
#if ((CONFIG_IED_REGION_SIZE & (CONFIG_IED_REGION_SIZE - 1)) != 0)
# error "CONFIG_IED_REGION_SIZE is not a power of 2"
#endif

/* Lock MSRs */
void intel_model_206ax_finalize_smm(void);

/* Configure power limits for turbo mode */
void set_power_limits(u8 power_limit_1_time);
int cpu_config_tdp_levels(void);
int get_platform_id(void);

static inline u8 cpu_stepping(void)
{
	return cpuid_eax(1) & 0xf;
}

#endif
