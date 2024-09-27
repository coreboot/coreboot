/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_P2SB_H_
#define _SOC_SNOWRIDGE_P2SB_H_

#include <commonlib/bsd/helpers.h>

#define PCH_P2SB_EPMASK0 0x220

#define P2SB_BAR  CONFIG_PCR_BASE_ADDRESS
#define P2SB_SIZE (16 * MiB)

#define HPTC_OFFSET          0x60
#define HPTC_ADDR_ENABLE_BIT BIT(7)

#endif /* _SOC_SNOWRIDGE_P2SB_H_ */
