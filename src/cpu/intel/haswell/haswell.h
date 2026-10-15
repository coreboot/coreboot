/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_HASWELL_H
#define _CPU_INTEL_HASWELL_H

#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <stdbool.h>
#include <stdint.h>

/* CPU types without stepping */
#define HASWELL_FAMILY_TRAD	0x306c0
#define HASWELL_FAMILY_ULT	0x40650
#define CRYSTALWELL_FAMILY	0x40660
#define BROADWELL_FAMILY_TRAD	0x40670
#define BROADWELL_FAMILY_ULT	0x306d0

/* Haswell CPUIDs */
#define CPUID_HASWELL_A0	0x306c1
#define CPUID_HASWELL_B0	0x306c2
#define CPUID_HASWELL_C0	0x306c3

#define CPUID_HASWELL_ULT_B0	0x40650
#define CPUID_HASWELL_ULT_C0	0x40651

/* Crystalwell CPUIDs */
#define CPUID_CRYSTALWELL_B0	0x40660
#define CPUID_CRYSTALWELL_C0	0x40661

/* Broadwell CPUIDs */
#define CPUID_BROADWELL_G0	0x40671

#define CPUID_BROADWELL_ULT_C0	0x306d2
#define CPUID_BROADWELL_ULT_D0	0x306d3
#define CPUID_BROADWELL_ULT_E0	0x306d4

/* Haswell and Broadwell bus clock is fixed at 100MHz */
#define CPU_BCLK			100

#define MSR_CORE_THREAD_COUNT		0x35
#define MSR_PLATFORM_INFO		0xce
#define MSR_PKG_CST_CONFIG_CONTROL	0xe2
#define MSR_PMG_IO_CAPTURE_BASE		0xe4
#define MSR_FEATURE_CONFIG		0x13c
#define SMM_MCA_CAP_MSR			0x17d
#define   SMM_CPU_SVRSTR_BIT		57
#define   SMM_CPU_SVRSTR_MASK		(1 << (SMM_CPU_SVRSTR_BIT - 32))
#define   SMM_CODE_ACCESS_CHK_BIT	58
#define   SMM_CODE_ACCESS_CHK_MASK	(1 << (SMM_CODE_ACCESS_CHK_BIT - 32))
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
#define   SMM_FEATURE_CONTROL_LOCK	(1 << 0)
#define   SMM_CPU_SAVE_EN		(1 << 1)
#define   SMM_CODE_CHK_EN		(1 << 2)

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
#define  PKG_POWER_LIMIT_LOCK		(1U << 31)

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

union msr_platform_info {
	struct __packed {
		uint32_t                            : 8; /* Bits  7:0  */
		uint32_t max_non_turbo_ratio        : 8; /* Bits 15:8  */
		uint32_t smm_save_control           : 1; /* Bits 16:16 */
		uint32_t                            : 7; /* Bits 23:17 */
		uint32_t oc_voltage_ovrd_avail      : 1; /* Bits 24:24 */
		uint32_t fivr_rfi_tuning_avail      : 1; /* Bits 25:25 */
		uint32_t dcu_mode_select_support    : 1; /* Bits 26:26 */
		uint32_t cpu_is_preprod_sample      : 1; /* Bits 27:27 */
		uint32_t prog_ratio_for_turbo       : 1; /* Bits 28:28 */
		uint32_t prog_tdp_for_turbo         : 1; /* Bits 29:29 */
		uint32_t prog_tcc_activation_offset : 1; /* Bits 30:30 */
		uint32_t cpuid_faulting_capability  : 1; /* Bits 31:31 */
		uint32_t low_power_mode_support     : 1; /* Bits 32:32 */
		uint32_t configurable_tdp_levels    : 2; /* Bits 34:33 */
		uint32_t bios_guard_supported       : 1; /* Bits 35:35 */
		uint32_t                            : 1; /* Bits 36:36 */
		uint32_t timed_mwait_supported      : 1; /* Bits 37:37 */
		uint32_t                            : 2; /* Bits 39:38 */
		uint32_t max_efficiency_ratio       : 8; /* Bits 47:40 */
		uint32_t minimum_ratio              : 8; /* Bits 55:48 */
		uint32_t                            : 8; /* Bits 63:56 */
	};
	msr_t msr;
};

union msr_vr_current_config {
	struct __packed {
		uint32_t current_limit      : 13; /* Bits 12:0  */
		uint32_t                    : 18; /* Bits 30:13 */
		uint32_t current_limit_lock :  1; /* Bits 31:31 */
		uint32_t psi1_threshold     : 10; /* Bits 41:32 */
		uint32_t psi2_threshold     : 10; /* Bits 51:42 */
		uint32_t psi3_threshold     : 10; /* Bits 61:52 */
		uint32_t psi4_enable        :  1; /* Bits 62:62 */
		uint32_t                    :  1; /* Bits 63:63 */
	};
	msr_t msr;
};

union msr_vr_misc_config {
	struct __packed {
		uint32_t dynamic_load_line_r  :  8; /* Bits  7:0  */
		uint32_t idle_load_line_r     :  8; /* Bits 15:8  */
		uint32_t leakage_load_line_r  :  8; /* Bits 23:16 */
		uint32_t min_vid              :  8; /* Bits 31:24 */
		int32_t  iout_offset          :  8; /* Bits 39:32 */
		uint32_t iout_slope           : 10; /* Bits 49:40 */
		uint32_t fast_idle_exit_ramp  :  1; /* Bits 50:50 */
		uint32_t fast_idle_entry_ramp :  1; /* Bits 51:51 */
		uint32_t idle_entry_decay_en  :  1; /* Bits 52:52 */
		uint32_t prog_slow_ramp_rate  :  2; /* Bits 54:53 */
		uint32_t                      :  9; /* Bits 63:55 */
	};
	msr_t msr;
};

/* Configure power limits for turbo mode */
void set_power_limits(u8 power_limit_1_time);
int cpu_config_tdp_levels(void);

void set_max_freq(void);

/* pcode_mailbox.c */
int pcode_ready(void);
u32 pcode_mailbox_read(u32 command);
int pcode_mailbox_write(u32 command, u32 data);

/* report_cpu_info.c */
void report_cpu_info(void);

/* smm_finalize.c */
void enable_smm_code_access_check(void);

static inline union msr_platform_info rdmsr_platform_info(void)
{
	return (union msr_platform_info) {
		.msr = rdmsr(MSR_PLATFORM_INFO),
	};
}

/* CPU identification */
static inline u32 cpu_family_model(void)
{
	return cpuid_eax(1) & 0x0fff0ff0;
}

static inline u32 cpu_stepping(void)
{
	return cpuid_eax(1) & 0xf;
}

static inline bool haswell_is_ult(void)
{
	return CONFIG(INTEL_LYNXPOINT_LP);
}

static inline bool cpu_is_haswell(void)
{
	switch (cpu_family_model()) {
	case HASWELL_FAMILY_TRAD:
	case HASWELL_FAMILY_ULT:
	case CRYSTALWELL_FAMILY:
		return true;
	default:
		return false;
	}
}

static inline bool cpu_is_broadwell(void)
{
	switch (cpu_family_model()) {
	case BROADWELL_FAMILY_TRAD:
	case BROADWELL_FAMILY_ULT:
		return true;
	default:
		return false;
	}
}

#endif
