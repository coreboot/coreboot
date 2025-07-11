/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_COMMON_TIMER_REG_H
#define SOC_MEDIATEK_COMMON_TIMER_REG_H

#include <device/mmio.h>
#include <soc/addressmap.h>

#define SYST_CON_EN			BIT(0)
#define SYST_CON_IRQ_CLR		BIT(4)
#define REV_CLR_EN			0x3
#define COMP_FEATURE_CLR		0
#define SYSTIMER_CNT			8
#define SYST_CON_CLR			0

struct systimer {
	u32 cntcr;
	u32 reserved;
	u32 cntcv_l;
	u32 cntcv_h;
	u32 reserved1[12];
	struct {
		u32 con;
		u32 val;
	} cnttval[SYSTIMER_CNT];
};
check_member(systimer, cntcr, 0x0);
check_member(systimer, cntcv_l, 0x0008);
check_member(systimer, cntcv_h, 0x000C);
check_member(systimer, cnttval[0].con, 0x0040);
check_member(systimer, cnttval[0].val, 0x0044);

static struct systimer *mtk_systimer = (void *)SYSTIMER_BASE;

DEFINE_BITFIELD(COMP_FEATURE, 12, 10)
DEFINE_BITFIELD(REV_SET, 18, 17)
DEFINE_BITFIELD(SYST_CON, 4, 0)

#endif
