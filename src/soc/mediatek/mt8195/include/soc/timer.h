/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8195_TIMER_H
#define SOC_MEDIATEK_MT8195_TIMER_H

#include <soc/timer_v2.h>

enum {
	TIE_0_EN = 1 << 3,
	COMP_15_EN = 1 << 10,
	COMP_20_EN = 1 << 11,
	COMP_25_EN = 1 << 12,

	COMP_FEATURE_MASK = COMP_15_EN | COMP_20_EN | COMP_25_EN | TIE_0_EN,

	COMP_15_MASK = COMP_15_EN,
	COMP_20_MASK = COMP_20_EN | TIE_0_EN,
	COMP_25_MASK = COMP_20_EN | COMP_25_EN,
};
#endif
