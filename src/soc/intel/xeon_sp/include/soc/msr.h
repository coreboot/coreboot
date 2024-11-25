/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#include <intelblocks/msr.h>

#define MSR_FEATURE_CONFIG              0x13c
#define   FEATURE_CONFIG_LOCK           BIT(0)

#define MSR_SNC_CONFIG                  0x152

#define IA32_MCG_CAP                    0x179
#define IA32_MCG_CAP_COUNT_MASK         0xff
#define IA32_MCG_CAP_CTL_P_BIT          8
#define IA32_MCG_CAP_CTL_P_MASK         (1 << IA32_MCG_CAP_CTL_P_BIT)

#define IA32_MCG_CTL                    0x17b /* IA32_MCG_CAP[MCG_CTL_P] == 1 */

/* IA32_MISC_ENABLE bits */
#define FAST_STRINGS_ENABLE_BIT         (1 << 0)
#define SPEED_STEP_ENABLE_BIT           (1 << 16)
#define MONIOR_ENABLE_BIT               (1 << 18)

#define MSR_IA32_ENERGY_PERF_BIAS       0x1b0

/* MSR_PKG_CST_CONFIG_CONTROL bits */
#define MSR_PKG_CST_CONFIG_CONTROL       0xe2
#define PKG_CSTATE_LIMIT_SHIFT           0 /* 0:3 */
/* No package C-state limit. All C-States supported by the processor are available. */
#define PKG_CSTATE_LIMIT_MASK            (0xf << PKG_CSTATE_LIMIT_SHIFT)
#define CFG_LOCK_SHIFT                   15
#define CFG_LOCK_ENABLE                  (1 << CFG_LOCK_SHIFT)

/* MSR_POWER_CTL bits */
#define MSR_POWER_CTL                            0x1fc
#define BIDIR_PROCHOT_ENABLE_SHIFT               0
#define BIDIR_PROCHOT_ENABLE                     (1 << BIDIR_PROCHOT_ENABLE_SHIFT)
#define C1E_ENABLE_SHIFT	     1
#define C1E_ENABLE		     (1 << C1E_ENABLE_SHIFT)

/* MSR_IA32_PERF_CTRL (0x199) bits */
#define MSR_IA32_PERF_CTRL          0x199
#define PSTATE_REQ_SHIFT            8 /* 8:14 bits */
#define PSTATE_REQ_MASK             (0x7f << PSTATE_REQ_SHIFT)
#define PSTATE_REQ_RATIO            (0xa << PSTATE_REQ_SHIFT)

/* MSR_MISC_PWR_MGMT bits */
#define MSR_MISC_PWR_MGMT            0x1aa
#define HWP_ENUM_SHIFT               6
#define HWP_ENUM_ENABLE              (1 << HWP_ENUM_SHIFT)
#define HWP_EPP_SHIFT                12
#define HWP_EPP_ENUM_ENABLE          (1 << HWP_EPP_SHIFT)
#define LOCK_MISC_PWR_MGMT_MSR_SHIFT 13
#define LOCK_MISC_PWR_MGMT_MSR       (1 << LOCK_MISC_PWR_MGMT_MSR_SHIFT)
#define LOCK_THERM_INT_SHIFT         22
#define LOCK_THERM_INT               (1 << LOCK_THERM_INT_SHIFT)

/* MSR_TURBO_RATIO_LIMIT bits */
#define MSR_TURBO_RATIO_LIMIT        0x1ad

/* MSR_TURBO_RATIO_LIMIT_CORES (0x1ae) */
#define MSR_TURBO_RATIO_LIMIT_CORES  0x1ae

/* MSR_VR_CURRENT_CONFIG bits */
#define MSR_VR_CURRENT_CONFIG        0x601
#define CURRENT_LIMIT_LOCK_SHIFT     31
#define CURRENT_LIMIT_LOCK           (0x1 << CURRENT_LIMIT_LOCK_SHIFT)

#define MSR_VR_MISC_CONFIG 0x603

/* MSR_TURBO_ACTIVATION_RATIO bits */
#define MSR_TURBO_ACTIVATION_RATIO   0x64c
#define MAX_NON_TURBO_RATIO_SHIFT    0
#define MAX_NON_TURBO_RATIO          (0xff << MAX_NON_TURBO_RATIO_SHIFT)
#define IA32_PM_ENABLE 0x770
#define IA32_HWP_CAPABILITIES 0x771

/* MSR_ENERGY_PERF_BIAS_CONFIG bits */
#define MSR_ENERGY_PERF_BIAS_CONFIG  0xa01
#define EPB_ENERGY_POLICY_SHIFT      3
#define EPB_ENERGY_POLICY_MASK       (0xf << EPB_ENERGY_POLICY_SHIFT)

/* MSR Protected Processor Inventory Number */
#define MSR_PPIN_CTL		     0x04e
#define MSR_PPIN_CTL_LOCK	     0x1
#define MSR_PPIN_CTL_ENABLE_SHIFT    1
#define MSR_PPIN_CTL_ENABLE	     (0x1 << MSR_PPIN_CTL_ENABLE_SHIFT)
#define MSR_PPIN		     0x04f
#define MSR_PPIN_CAP_SHIFT	     23
#define MSR_PPIN_CAP		     (0x1 << MSR_PPIN_CAP_SHIFT)

/* SOC-specific #defines may use the above definitions */
#include <soc/soc_msr.h>

#endif /* _SOC_MSR_H_ */
