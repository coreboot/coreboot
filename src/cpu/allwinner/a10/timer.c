/*
 * Timer control and delays for Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "timer.h"

#include <arch/io.h>
#include <delay.h>
#include <timer.h>

struct a1x_timer_module *const timer_module = (void *)A1X_TIMER_BASE;
struct a1x_timer *const tmr0 =
		&((struct a1x_timer_module *)A1X_TIMER_BASE)->timer[0];

static inline u32 read_timer(void)
{
	return read32(&tmr0->val);
}

void init_timer(void)
{
	u32 reg32;
	/* Load the timer rollover value */
	write32(&tmr0->interval, 0xffffffff);
	/* Configure the timer to run from 24MHz oscillator, no prescaler */
	reg32 = TIMER_CTRL_PRESC_DIV_EXP(0);
	reg32 |= TIMER_CTRL_CLK_SRC_OSC24M;
	reg32 |= TIMER_CTRL_RELOAD;
	reg32 |= TIMER_CTRL_TMR_EN;
	write32(&tmr0->ctrl, reg32);
}

void udelay(unsigned usec)
{
	u32 curr_tick, last_tick;
	s32 ticks_left;

	last_tick = read_timer();
	/* 24 timer ticks per microsecond (24 MHz, divided by 1) */
	ticks_left = usec * 24;

	/* FIXME: Should we consider timer rollover?
	 * From when we start the timer, we have almost three minutes before it
	 * rolls over, so we should be long into having booted our payload.
	 */
	while (ticks_left > 0) {
		curr_tick = read_timer();
		/* Timer value decreases with each tick */
		ticks_left -= last_tick - curr_tick;
		last_tick = curr_tick;
	}

}

/*
 * This function has nothing to do with timers; however, the chip revision
 * register is in the timer module, so keep this function here.
 */
u8 a1x_get_cpu_chip_revision(void)
{
	write32(&timer_module->cpu_cfg, 0);
	return (read32(&timer_module->cpu_cfg) >> 6) & 0x3;
}
