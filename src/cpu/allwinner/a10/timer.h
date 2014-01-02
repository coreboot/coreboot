/*
 * Definitions for timer control on Allwinner CPUs
 *
 * Copyright (C) 2007-2011 Allwinner Technology Co., Ltd.
 *	Tom Cubie <tangliang@allwinnertech.com>
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_TIMER_H
#define CPU_ALLWINNER_A10_TIMER_H

#include "memmap.h"
#include <types.h>

/* TMRx_CTRL values */
#define TIMER_CTRL_MODE_SINGLE		(1 << 7)
#define TIMER_CTRL_PRESC_MASK		(0x7 << 4)
#define  TIMER_CTRL_PRESC_DIV_EXP(ep)	((ep << 4) & TIMER_CTRL_PRESC_MASK)
#define TIMER_CTRL_CLK_SRC_MASK		(0x3 << 2)
#define  TIMER_CTRL_CLK_SRC_LOSC	(0x0 << 2)
#define  TIMER_CTRL_CLK_SRC_OSC24M	(0x1 << 2)
#define  TIMER_CTRL_CLK_SRC_PLL6	(0x2 << 2)
#define TIMER_CTRL_RELOAD		(1 << 1)
#define TIMER_CTRL_TMR_EN		(1 << 0)

/* Chip revision definitions (found in CPU_CFG register) */
#define A1X_CHIP_REV_A			0x0
#define A1X_CHIP_REV_C1			0x1
#define A1X_CHIP_REV_C2			0x2
#define A1X_CHIP_REV_B			0x3


/* General purpose timer */
struct a1x_timer {
	u32 ctrl;
	u32 interval;
	u32 val;
	u8 res[4];
} __attribute__ ((packed));

/* Audio video sync*/
struct a1x_avs {
	u32 ctrl;		/* 0x80 */
	u32 cnt0;		/* 0x84 */
	u32 cnt1;		/* 0x88 */
	u32 div;		/* 0x8c */
} __attribute__ ((packed));

/* Watchdog */
struct a1x_wdog {
	u32 ctrl;		/* 0x90 */
	u32 mode;		/* 0x94 */
} __attribute__ ((packed));

/* 64 bit counter */
struct a1x_64cnt {
	u32 ctrl;		/* 0xa0 */
	u32 lo;			/* 0xa4 */
	u32 hi;			/* 0xa8 */
} __attribute__ ((packed));

/* Rtc */
struct a1x_rtc {
	u32 ctrl;		/* 0x100 */
	u32 yymmdd;		/* 0x104 */
	u32 hhmmss;		/* 0x108 */
} __attribute__ ((packed));

/* Alarm */
struct a1x_alarm {
	u32 ddhhmmss;		/* 0x10c */
	u32 hhmmss;		/* 0x110 */
	u32 en;			/* 0x114 */
	u32 irq_en;		/* 0x118 */
	u32 irq_sta;		/* 0x11c */
} __attribute__ ((packed));

struct a1x_timer_module {
	u32 irq_en;		/* 0x00 */
	u32 irq_sta;		/* 0x04 */
	u8 res1[8];
	struct a1x_timer timer[6];	/* We have 6 timers */
	u8 res2[16];
	struct a1x_avs avs;
	struct a1x_wdog wdog;
	u8 res3[8];
	struct a1x_64cnt cnt64;
	u8 res4[0x58];
	struct a1x_rtc rtc;
	struct a1x_alarm alarm;
	u32 gp_data[4];
	u8 res5[8];
	u32 cpu_cfg;
} __attribute__ ((packed));

u8 a1x_get_cpu_chip_revision(void);

#endif				/* CPU_ALLWINNER_A10_TIMER_H */
