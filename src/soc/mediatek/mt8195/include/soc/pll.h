/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8195_PLL_H
#define SOC_MEDIATEK_MT8195_PLL_H

#include <soc/pll_common.h>

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
};

/* top_mux rate */
enum {
	UART_HZ		= CLK26M_HZ,
};

#endif /* SOC_MEDIATEK_MT8195_PLL_H */
