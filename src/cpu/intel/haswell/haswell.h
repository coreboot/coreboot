/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_HASWELL_H
#define _CPU_INTEL_HASWELL_H

#include <stdint.h>

/* Haswell CPU types */
#define HASWELL_FAMILY_MOBILE		0x306c0
#define HASWELL_FAMILY_ULT		0x40650

/* Haswell CPU steppings */
#define HASWELL_STEPPING_MOBILE_A0	1
#define HASWELL_STEPPING_MOBILE_B0	2
#define HASWELL_STEPPING_MOBILE_C0	3
#define HASWELL_STEPPING_MOBILE_D0	4
#define HASWELL_STEPPING_ULT_B0		0
#define HASWELL_STEPPING_ULT_C0		1

/* Haswell bus clock is fixed at 100MHz */
#define CPU_BCLK			100

#define MSR_CORE_THREAD_COUNT		0x35
#define MSR_PLATFORM_INFO		0xce
#define  PLATFORM_INFO_SET_TDP		(1 << 29)
#define MSR_PKG_CST_CONFIG_CONTROL	0xe2
#define MSR_PMG_IO_CAPTURE_BASE		0xe4
#define MSR_FEATURE_CONFIG		0x13c
#define SMM_MCA_CAP_MSR			0x17d
#define   SMM_CPU_SVRSTR_BIT		57
#define   SMM_CPU_SVRSTR_MASK		(1 << (SMM_CPU_SVRSTR_BIT - 32))
#define MSR_FLEX_RATIO			0x194
#define  FLEX_RATIO_LOCK		(1 << 20)
#define  FLEX_RATIO_EN			(1 << 16)
#define MSR_TEMPERATURE_TARGET		0x1a2
#define MSR_MISC_PWR_MGMT		0x1aa
#define  MISC_PWR_MGMT_EIST_HW_DIS	(1 << 0)
#define MSR_TURBO_RATIO_LIMIT		0x1ad
#define MSR_PRMRR_PHYS_BASE		0x1f4
#define MSR_PRMRR_PHYS_MASK		0x1f5
#define MSR_POWER_CTL			0x1fc
#define MSR_LT_LOCK_MEMORY		0x2e7
#define MSR_UNCORE_PRMRR_PHYS_BASE	0x2f4
#define MSR_UNCORE_PRMRR_PHYS_MASK	0x2f5
#define SMM_FEATURE_CONTROL_MSR		0x4e0
#define   SMM_CPU_SAVE_EN		(1 << 1)

#define MSR_C_STATE_LATENCY_CONTROL_0	0x60a
#define MSR_C_STATE_LATENCY_CONTROL_1	0x60b
#define MSR_C_STATE_LATENCY_CONTROL_2	0x60c
#define MSR_C_STATE_LATENCY_CONTROL_3	0x633
#define MSR_C_STATE_LATENCY_CONTROL_4	0x634
#define MSR_C_STATE_LATENCY_CONTROL_5	0x635
#define  IRTL_VALID			(1 << 15)
#define  IRTL_1_NS			(0 << 10)
#define  IRTL_32_NS			(1 << 10)
#define  IRTL_1024_NS			(2 << 10)
#define  IRTL_32768_NS			(3 << 10)
#define  IRTL_1048576_NS		(4 << 10)
#define  IRTL_33554432_NS		(5 << 10)
#define  IRTL_RESPONSE_MASK		(0x3ff)

/* Long duration in low dword, short duration in high dword */
#define MSR_PKG_POWER_LIMIT		0x610
#define  PKG_POWER_LIMIT_MASK		0x7fff
#define  PKG_POWER_LIMIT_EN		(1 << 15)
#define  PKG_POWER_LIMIT_CLAMP		(1 << 16)
#define  PKG_POWER_LIMIT_TIME_SHIFT	17
#define  PKG_POWER_LIMIT_TIME_MASK	0x7f

#define MSR_VR_CURRENT_CONFIG		0x601
#define MSR_VR_MISC_CONFIG		0x603
#define MSR_PKG_POWER_SKU_UNIT		0x606
#define MSR_PKG_POWER_SKU		0x614
#define MSR_DDR_RAPL_LIMIT		0x618
#define MSR_VR_MISC_CONFIG2		0x636
#define MSR_PP0_POWER_LIMIT		0x638
#define MSR_PP1_POWER_LIMIT		0x640

#define MSR_CONFIG_TDP_NOMINAL		0x648
#define MSR_CONFIG_TDP_LEVEL1		0x649
#define MSR_CONFIG_TDP_LEVEL2		0x64a
#define MSR_CONFIG_TDP_CONTROL		0x64b
#define MSR_TURBO_ACTIVATION_RATIO	0x64c

/* SMM save state MSRs */
#define SMBASE_MSR			0xc20
#define IEDBASE_MSR			0xc22

/* MTRR_CAP_MSR bit definitions */
#define SMRR_SUPPORTED			(1 << 11)
#define PRMRR_SUPPORTED			(1 << 12)

/* Intel suggested latency times in units of 1024ns. */
#define C_STATE_LATENCY_CONTROL_0_LIMIT 0x42
#define C_STATE_LATENCY_CONTROL_1_LIMIT 0x73
#define C_STATE_LATENCY_CONTROL_2_LIMIT 0x91
#define C_STATE_LATENCY_CONTROL_3_LIMIT 0xe4
#define C_STATE_LATENCY_CONTROL_4_LIMIT 0x145
#define C_STATE_LATENCY_CONTROL_5_LIMIT 0x1ef

#define C_STATE_LATENCY_MICRO_SECONDS(limit, base) \
	(((1 << ((base) * 5)) * (limit)) / 1000)
#define C_STATE_LATENCY_FROM_LAT_REG(reg) \
	C_STATE_LATENCY_MICRO_SECONDS(C_STATE_LATENCY_CONTROL_ ##reg## _LIMIT, \
				      (IRTL_1024_NS >> 10))

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

/*
 * List of supported C-states for Haswell and Broadwell.
 * Only the ULT parts support C8, C9, and C10.
 */
enum {
	C_STATE_C0		=  0,
	C_STATE_C1		=  1,
	C_STATE_C1E		=  2,
	C_STATE_C3		=  3,
	C_STATE_C6_SHORT_LAT	=  4,
	C_STATE_C6_LONG_LAT	=  5,
	C_STATE_C7_SHORT_LAT	=  6,
	C_STATE_C7_LONG_LAT	=  7,
	C_STATE_C7S_SHORT_LAT	=  8,
	C_STATE_C7S_LONG_LAT	=  9,
	C_STATE_C8		= 10,
	C_STATE_C9		= 11,
	C_STATE_C10		= 12,
	NUM_C_STATES,
};

/* Lock MSRs */
void intel_cpu_haswell_finalize_smm(void);

/* Configure power limits for turbo mode */
void set_power_limits(u8 power_limit_1_time);
int cpu_config_tdp_levels(void);

/* CPU identification */
static inline int haswell_is_ult(void)
{
	return CONFIG(INTEL_LYNXPOINT_LP);
}

#endif
