/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 1.2 2.2
 */

#ifndef SOC_MEDIATEK_MT8196_TIMER_H
#define SOC_MEDIATEK_MT8196_TIMER_H

#include <soc/timer_reg.h>
#include <soc/timer_v2.h>

DEFINE_BITFIELD(COMP_FEATURE_TIE, 4, 3)

#define COMP_FEATURE_20_EN		0x2
#define COMP_FEATURE_TIE_EN		0x1
#define COMP_FEATURE_TIE_CLR		0

#endif
