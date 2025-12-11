/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_DRAMC_INFO_H__
#define __SOC_MEDIATEK_COMMON_DRAMC_INFO_H__

#include <commonlib/bsd/helpers.h>

#define HW_TX_TRACING_BUF_SIZE	(64 * KiB)

void reserve_buffer_for_dramc(void);

#endif	/* __SOC_MEDIATEK_COMMON_DRAMC_INFO_H__ */
