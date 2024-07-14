/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 1.2 2.2
 */

#ifndef SOC_MEDIATEK_MT8196_TIMER_H
#define SOC_MEDIATEK_MT8196_TIMER_H

#include <soc/timer_v2.h>
#include <stdint.h>

DEFINE_BITFIELD(COMP_FEATURE, 12, 10)
DEFINE_BITFIELD(COMP_FEATURE_TIE, 4, 3)
DEFINE_BITFIELD(REV_SET, 18, 17)
DEFINE_BITFIELD(SYST_CON, 4, 0)

#define SYST_CON_EN			BIT(0)
#define SYST_CON_IRQ_CLR		BIT(4)
#define REV_CLR_EN			0x3
#define COMP_FEATURE_20_EN		0x2
#define COMP_FEATURE_TIE_EN		0x1
#define COMP_FEATURE_CLR                0
#define COMP_FEATURE_TIE_CLR		0
#define SYSTIMER_CNT			8
#define SYST_CON_CLR			0

struct systimer {
	u32 cntcr;
	u32 reserved;
	u32 cntcv_l;
	u32 cntcv_h;
	u32 reserved1[0x30];
	struct {
		u32 con;
		u32 val;
	} cnttval[SYSTIMER_CNT];
};

check_member(systimer, cntcr, 0x0);
check_member(systimer, cntcv_l, 0x0008);
check_member(systimer, cntcv_h, 0x000c);

#endif
