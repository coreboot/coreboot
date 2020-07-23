/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#include <intelblocks/msr.h>

#define IA32_MCG_CAP                    0x179
#define IA32_MCG_CAP_COUNT_MASK         0xff
#define IA32_MCG_CAP_CTL_P_BIT          8
#define IA32_MCG_CAP_CTL_P_MASK         (1 << IA32_MCG_CAP_CTL_P_BIT)

#define IA32_MCG_CTL                    0x17b

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
#define PKG_CSTATE_NO_LIMIT              (0x7 << PKG_CSTATE_LIMIT_SHIFT)
#define IO_MWAIT_REDIRECTION_SHIFT       10
#define IO_MWAIT_REDIRECTION_ENABLE      (1 << IO_MWAIT_REDIRECTION_SHIFT)
#define CFG_LOCK_SHIFT                   15
#define CFG_LOCK_ENABLE                  (1 << CFG_LOCK_SHIFT)

/* MSR_PMG_IO_CAPTURE_BASE bits */
#define MSR_PMG_IO_CAPTURE_BASE          0xe4
#define LVL_2_BASE_ADDRESS_SHIFT         0 /* 15:0 bits */
#define LVL_2_BASE_ADDRESS               (0x0514 << LVL_2_BASE_ADDRESS_SHIFT)
#define CST_RANGE_SHIFT                  16 /* 18:16 bits */
#define CST_RANGE_MAX_C6                 (0x1 << CST_RANGE_SHIFT)

/* MSR_POWER_CTL bits */
#define MSR_POWER_CTL                            0x1fc
#define BIDIR_PROCHOT_ENABLE_SHIFT               0
#define BIDIR_PROCHOT_ENABLE                     (1 << BIDIR_PROCHOT_ENABLE_SHIFT)
#define FAST_BRK_SNP_ENABLE_SHIFT                3
#define FAST_BRK_SNP_ENABLE                      (1 << FAST_BRK_SNP_ENABLE_SHIFT)
#define FAST_BRK_INT_ENABLE_SHIFT                4
#define FAST_BRK_INT_ENABLE                      (1 << FAST_BRK_INT_ENABLE_SHIFT)
#define PHOLD_CST_PREVENTION_INIT_SHIFT          6
#define PHOLD_CST_PREVENTION_INIT_VALUE          (1 << PHOLD_CST_PREVENTION_INIT_SHIFT)
#define ENERGY_PERF_BIAS_ACCESS_ENABLE_SHIFT     18
#define ENERGY_PERF_BIAS_ACCESS_ENABLE           (1 << ENERGY_PERF_BIAS_ACCESS_ENABLE_SHIFT)
#define PROCHOT_OUTPUT_DISABLE_SHIFT             21
#define PROCHOT_OUTPUT_DISABLE                   (1 << PROCHOT_OUTPUT_DISABLE_SHIFT)
#define PWR_PERF_TUNING_DYN_SWITCHING_SHIFT      24
#define PWR_PERF_TUNING_DYN_SWITCHING_ENABLE     (1 << PWR_PERF_TUNING_DYN_SWITCHING_SHIFT)
#define PROCHOT_LOCK_SHIFT                       27
#define PROCHOT_LOCK_ENABLE                      (1 << PROCHOT_LOCK_SHIFT)
#define LTR_IIO_DISABLE_SHIFT                    29
#define LTR_IIO_DISABLE                          (1 << LTR_IIO_DISABLE_SHIFT)

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

/* MSR_TURBO_ACTIVATION_RATIO bits */
#define MSR_TURBO_ACTIVATION_RATIO   0x64c
#define MAX_NON_TURBO_RATIO_SHIFT    0
#define MAX_NON_TURBO_RATIO          (0xff << MAX_NON_TURBO_RATIO_SHIFT)

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

#endif /* _SOC_MSR_H_ */
