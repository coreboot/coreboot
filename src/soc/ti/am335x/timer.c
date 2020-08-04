/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>
#include <delay.h>
#include <device/mmio.h>

#include "dmtimer.h"
#include "clock.h"

struct am335x_dmtimer *dmtimer_2 = (struct am335x_dmtimer *)DMTIMER_2;

#define CLKSEL_M_OSC (0x01 << 0)

static uint32_t timer_raw_value(void)
{
	return read32(&dmtimer_2->tcrr);
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value() / M_OSC_MHZ);
}

void init_timer(void)
{
	write32(&am335x_cm_dpll->clksel_timer2_clk, CLKSEL_M_OSC);

	// Start the dmtimer in autoreload mode without any prescalers
	// With M_OSC at 24MHz, this gives a few minutes before the timer overflows
	write32(&dmtimer_2->tclr, TCLR_ST | TCLR_AR);
}
