/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SRC_SOC_CAVIUM_CN81XX_INCLUDE_CLOCK_H_
#define SRC_SOC_CAVIUM_CN81XX_INCLUDE_CLOCK_H_

#include <types.h>

u64 thunderx_get_ref_clock(void);
u64 thunderx_get_io_clock(void);
u64 thunderx_get_core_clock(void);

#endif /* SRC_SOC_CAVIUM_CN81XX_INCLUDE_CLOCK_H_ */
