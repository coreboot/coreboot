/* SPDX-License-Identifier: GPL-2.0-only */

#include <timer.h>
#include <delay.h>
#include <device/mmio.h>
#include <stdint.h>

#define G_TIMER_COUNTER_0 ((void *)0xF8F00200u)
#define G_TIMER_COUNTER_1 ((void *)0xF8F00204u)
#define G_TIMER_CTRL ((void *)0xF8F00208u)

#define CLK_PER_USEC 108

static inline uint64_t timer_raw_value(void)
{
	uint32_t upper, lower;
	do {
		upper = read32(G_TIMER_COUNTER_1);
		lower = read32(G_TIMER_COUNTER_0);
	} while (upper != read32(G_TIMER_COUNTER_1));

	return (((uint64_t)upper) << 32) | lower;
}

void init_timer(void)
{
	/* Disable timer */
	write32(G_TIMER_CTRL, read32(G_TIMER_CTRL) & ~0x1);
	write32(G_TIMER_COUNTER_0, 0);
	write32(G_TIMER_COUNTER_1, 0);
	/* Enable timer */
	write32(G_TIMER_CTRL, read32(G_TIMER_CTRL) | 0x1);
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value() / CLK_PER_USEC);
}
