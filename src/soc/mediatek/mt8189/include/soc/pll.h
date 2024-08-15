/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 8.1
 */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_PLL_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_PLL_H__

#include <soc/pll_common.h>

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
};

/* top_mux rate */
enum {
	UART_HZ		= CLK26M_HZ,
};

#endif /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_PLL_H__ */
