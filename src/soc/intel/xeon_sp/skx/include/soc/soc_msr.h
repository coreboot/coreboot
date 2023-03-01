/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_MSR_SKX_H_
#define _SOC_MSR_SKX_H_

/* MCA_ERROR_CONTROL */
#define U2C_SMI_ENABLED         (1 << 2)

/* IA32_ERR_CTRL */
#define CORE_ERR_DISABLE        (1 << 5)
#define CMCI_DISABLE            (1 << 4)
#define UCE_TO_CE_DOWNGRADE     (1 << 2)

/* MSR_PKG_CST_CONFIG_CONTROL */
#define PKG_CSTATE_NO_LIMIT     (0x7 << PKG_CSTATE_LIMIT_SHIFT)

/* MSR_POWER_CTL (SKX and CPX) */
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

#endif /* _SOC_MSR_SKX_H_ */
