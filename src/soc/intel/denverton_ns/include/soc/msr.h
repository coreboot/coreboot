/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_MSR_H_
#define _DENVERTON_NS_MSR_H_

#include <intelblocks/msr.h>

#define PLATFORM_INFO_SET_TDP (1 << 29)
#define MSR_FEATURE_CONFIG 0x13c
#define   FEATURE_CONFIG_RESERVED_MASK	0x3ULL
#define   FEATURE_CONFIG_LOCK	(1 << 0)
#define MSR_POWER_MISC		0x120
#define  ENABLE_IA_UNTRUSTED	(1 << 6)
#define IA32_MCG_CAP			0x179
#define  IA32_MCG_CAP_COUNT_MASK	0xff
#define  IA32_MCG_CAP_CTL_P_BIT		8
#define  IA32_MCG_CAP_CTL_P_MASK	(1 << IA32_MCG_CAP_CTL_P_BIT)
#define IA32_MCG_CTL			0x17b
/*      IA32_MISC_ENABLE 0x1a0 */
#define  THERMAL_MONITOR_ENABLE_BIT (1 << 3)
#define EMRR_PHYS_BASE_MSR 0x1f4
#define MSR_LT_LOCK_MEMORY 0x2e7
#define MSR_UNCORE_PRMRR_PHYS_BASE 0x2f4
#define MSR_UNCORE_PRMRR_PHYS_MASK 0x2f5

#define MSR_VR_CURRENT_CONFIG 0x601
#define MSR_VR_MISC_CONFIG 0x603
#define MSR_VR_MISC_CONFIG2 0x636
#define MSR_PP0_POWER_LIMIT 0x638
#define MSR_PP1_POWER_LIMIT 0x640

/* Read BCLK from MSR */
unsigned int bus_freq_khz(void);

#endif /* _DENVERTON_NS_MSR_H_ */
