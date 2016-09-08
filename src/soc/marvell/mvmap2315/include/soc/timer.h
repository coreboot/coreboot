/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_MARVELL_MVMAP2315_TIMER_H__
#define __SOC_MARVELL_MVMAP2315_TIMER_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define MVMAP2315_CLOCKS_PER_USEC	13

#define MVMAP2315_TIMER_T1CR_TE		BIT(0)
#define MVMAP2315_TIMER_T1CR_TM		BIT(1)
#define MVMAP2315_TIMER_T1CR_TIM	BIT(2)
#define MVMAP2315_TIMER_T1CR_TPWM	BIT(3)

struct mvmap2315_timer_regs {
	u32 t1lc;
	u32 t1cv;
	u32 t1cr;
	u32 t1eoi;
	u32 t1is;
	u8 _reserved0[0x8c];
	u32 tis;
	u32 teoi;
	u32 tris;
	u32 tcv;
	u32 t1lc2;
};

check_member(mvmap2315_timer_regs, t1lc2, 0xB0);
static struct mvmap2315_timer_regs * const mvmap2315_timer0
					= (void *)MVMAP2315_TIMER0_BASE;

#endif /* __SOC_MARVELL_MVMAP2315_TIMER_H__ */
