/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#include <intelblocks/msr.h>

#define MSR_POWER_MISC		0x120
#define  ENABLE_IA_UNTRUSTED	(1 << 6)
#define  FLUSH_DL1_L2		(1 << 8)

#endif
