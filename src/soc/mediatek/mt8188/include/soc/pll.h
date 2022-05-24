/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 4.1
 */

#ifndef SOC_MEDIATEK_MT8188_PLL_H
#define SOC_MEDIATEK_MT8188_PLL_H

#include <soc/pll_common.h>

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
};

/* top_mux rate */
enum {
	UART_HZ		= CLK26M_HZ,
};

#endif /* SOC_MEDIATEK_MT8188_PLL_H */
