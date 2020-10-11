/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#include <intelblocks/msr.h>

#define MSR_LT_LOCK_MEMORY		0x2e7
#define MSR_UNCORE_PRMRR_PHYS_BASE	0x2f4
#define MSR_UNCORE_PRMRR_PHYS_MASK	0x2f5
#define MSR_VR_CURRENT_CONFIG		0x601
#define MSR_VR_MISC_CONFIG		0x603
#define MSR_PL3_CONTROL                 0x615
#define MSR_VR_MISC_CONFIG2		0x636
#define MSR_PP0_POWER_LIMIT		0x638
#define MSR_PP1_POWER_LIMIT		0x640
#define MSR_PLATFORM_POWER_LIMIT        0x65c

#endif
