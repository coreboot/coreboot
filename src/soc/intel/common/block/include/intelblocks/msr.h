/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_MSR_H
#define SOC_INTEL_COMMON_MSR_H

#define MSR_CORE_THREAD_COUNT	0x35
#define MSR_PLATFORM_INFO	0xce
#define MSR_PKG_CST_CONFIG_CONTROL	0xe2
/* Set MSR_PKG_CST_CONFIG_CONTROL[3:0] for Package C-State limit */
#define  PKG_C_STATE_LIMIT_C2_MASK	0x2
/* Set MSR_PKG_CST_CONFIG_CONTROL[7:4] for Core C-State limit*/
#define  CORE_C_STATE_LIMIT_C10_MASK	0x70
/* Set MSR_PKG_CST_CONFIG_CONTROL[10] to IO redirect to MWAIT */
#define  IO_MWAIT_REDIRECT_MASK	0x400
/* Set MSR_PKG_CST_CONFIG_CONTROL[15] to lock CST_CFG [0-15] bits */
#define  CST_CFG_LOCK_MASK	0x8000
#define MSR_BIOS_UPGD_TRIG	0x7a
#define  SGX_ACTIVATE_BIT	(1)
#define MSR_PMG_IO_CAPTURE_BASE	0xe4
#define MSR_EMULATE_PM_TIMER	0x121
#define  EMULATE_DELAY_OFFSET_VALUE	20
#define  EMULATE_PM_TMR_EN	(1 << 16)
#define  EMULATE_DELAY_VALUE	0x13
#define SMM_MCA_CAP_MSR		0x17d
#define  SMM_CPU_SVRSTR_BIT	57
#define  SMM_CPU_SVRSTR_MASK	(1 << (SMM_CPU_SVRSTR_BIT - 32))
#define MSR_FLEX_RATIO		0x194
#define  FLEX_RATIO_LOCK		(1 << 20)
#define  FLEX_RATIO_EN			(1 << 16)
/* This is burst mode BIT 38 in IA32_MISC_ENABLE MSR at offset 1A0h */
#define BURST_MODE_DISABLE		(1 << 6)
#define MSR_TEMPERATURE_TARGET	0x1a2
#define  TEMPERATURE_TCC_MASK	0xf
#define  TEMPERATURE_TCC_SHIFT	24
#define MSR_PREFETCH_CTL	0x1a4
#define  PREFETCH_L1_DISABLE	(1 << 0)
#define  PREFETCH_L2_DISABLE	(1 << 2)
#define MSR_MISC_PWR_MGMT	0x1aa
#define  MISC_PWR_MGMT_EIST_HW_DIS	(1 << 0)
#define  MISC_PWR_MGMT_ISST_EN		(1 << 6)
#define  MISC_PWR_MGMT_ISST_EN_INT	(1 << 7)
#define  MISC_PWR_MGMT_ISST_EN_EPP	(1 << 12)
#define MSR_TURBO_RATIO_LIMIT		0x1ad
#define MSR_PRMRR_PHYS_BASE		0x1f4
#define MSR_PRMRR_PHYS_MASK		0x1f5
#define  PRMRR_PHYS_MASK_LOCK		(1 << 10)
#define  PRMRR_PHYS_MASK_VALID		(1 << 11)
#define MSR_PRMRR_VALID_CONFIG		0x1fb
#define MSR_POWER_CTL			0x1fc
#define  POWER_CTL_C1E_MASK		(1 << 1)
#define MSR_PRMRR_BASE_0		0x2a0
#define MSR_EVICT_CTL			0x2e0
#define MSR_LT_CONTROL			0x2e7
#define  LT_CONTROL_LOCK		(1 << 0)
#define MSR_SGX_OWNEREPOCH0		0x300
#define MSR_SGX_OWNEREPOCH1		0x301
#define SMM_FEATURE_CONTROL_MSR		0x4e0
#define  SMM_CPU_SAVE_EN		(1 << 1)
#define MSR_PKG_POWER_SKU_UNIT		0x606
#define MSR_C_STATE_LATENCY_CONTROL_0	0x60a
#define MSR_C_STATE_LATENCY_CONTROL_1	0x60b
#define MSR_C_STATE_LATENCY_CONTROL_2	0x60c
#define MSR_PKG_POWER_LIMIT		0x610
/*
 * For Mobile, RAPL default PL1 time window value set to 28 seconds.
 * RAPL time window calculation defined as follows:
 * Time Window = (float)((1+X/4)*(2*^Y), X Corresponds to [23:22],
 * Y to [21:17] in MSR 0x610. 28 sec is equal to 0x6e.
 */
#define  MB_POWER_LIMIT1_TIME_DEFAULT	0x6e
#define MSR_PKG_POWER_SKU		0x614
#define MSR_DDR_RAPL_LIMIT		0x618
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
#define MSR_COUNTER_24_MHZ		0x637
#define MSR_CONFIG_TDP_NOMINAL		0x648
#define MSR_CONFIG_TDP_LEVEL1		0x649
#define MSR_CONFIG_TDP_LEVEL2		0x64a
#define MSR_CONFIG_TDP_CONTROL		0x64b
#define MSR_TURBO_ACTIVATION_RATIO	0x64c
#define PKG_POWER_LIMIT_MASK		(0x7fff)
#define PKG_POWER_LIMIT_EN		(1 << 15)
#define PKG_POWER_LIMIT_CLAMP		(1 << 16)
#define PKG_POWER_LIMIT_TIME_SHIFT	17
#define PKG_POWER_LIMIT_TIME_MASK	(0x7f)
#define PKG_POWER_LIMIT_DUTYCYCLE_SHIFT 24
#define PKG_POWER_LIMIT_DUTYCYCLE_MASK  (0x7f)

#define MSR_CORE_MKTME_ACTIVATION	0x9ff
/* SMM save state MSRs */
#define SMBASE_MSR			0xc20
#define IEDBASE_MSR			0xc22

#define MSR_L2_QOS_MASK(reg)		(0xd10 + reg)

/* MTRR_CAP_MSR bits */
#define SMRR_SUPPORTED			(1<<11)
#define PRMRR_SUPPORTED			(1<<12)
#define SMRR_LOCK_SUPPORTED		(1<<14)

#define SGX_SUPPORTED			(1<<2)
#define TME_SUPPORTED			(1<<13)

#define KEYLOCKER_SUPPORTED		(1<<23)
#define KEYLOCKER_AESKL			(1)

#endif	/* SOC_INTEL_COMMON_MSR_H */
