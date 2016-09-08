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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <delay.h>
#include <soc/timer.h>
#include <timer.h>

void init_timer(void)
{
	/* disable timer */
	clrbits_le32(&mvmap2315_timer0->t1cr, MVMAP2315_TIMER_T1CR_TE);

	/* set to free-running mode (loads max value at timer expiration) */
	clrbits_le32(&mvmap2315_timer0->t1cr, MVMAP2315_TIMER_T1CR_TM);

	/* mask interrupt (not currently used) */
	setbits_le32(&mvmap2315_timer0->t1cr, MVMAP2315_TIMER_T1CR_TIM);

	/* disable PWM output */
	clrbits_le32(&mvmap2315_timer0->t1cr, MVMAP2315_TIMER_T1CR_TPWM);

	/* perform dummy read to clear all active interrupts */
	read32(&mvmap2315_timer0->t1eoi);

	/* must provide an initial load count even in free-running mode */
	write32(&mvmap2315_timer0->t1lc, 0xFFFFFFFF);

	/* enable timer */
	setbits_le32(&mvmap2315_timer0->t1cr, MVMAP2315_TIMER_T1CR_TE);

	/* busy wait until timer count is non-zero */
	while (!read32(&mvmap2315_timer0->t1cv))
		;
}

static u32 timer_raw_value(void)
{
	/* invert count to change from down to up count */
	return ~read32(&mvmap2315_timer0->t1cv);
}

void timer_monotonic_get(struct mono_time *mt)
{
	mt->microseconds = (long)(timer_raw_value() /
				  MVMAP2315_CLOCKS_PER_USEC);
}
