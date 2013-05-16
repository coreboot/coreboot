/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <timer.h>
#include <delay.h>
#include "timer.h"
#include "pwm.h"
#include "clk.h"
#include "cpu.h"

static unsigned long long timer_reset_value;
static unsigned long lastinc;

/* macro to read the 16 bit timer */
static inline struct s5p_timer *s5p_get_base_timer(void)
{
	return samsung_get_base_timer();
}

/**
 * Read the countdown timer.
 *
 * This operates at 1MHz and counts downwards. It will wrap about every
 * hour (2^32 microseconds).
 *
 * @return current value of timer
 */
static unsigned long timer_get_us_down(void)
{
	struct s5p_timer *const timer = s5p_get_base_timer();

	return readl(&timer->tcnto4);
}

void init_timer(void)
{
	/* Timer may have been enabled in SPL */
	if (!pwm_check_enabled(4)) {
		/* PWM Timer 4 */
		pwm_init(4, MUX_DIV_4, 0);
		pwm_config(4, 100000, 100000);
		pwm_enable(4);

		/* Use this as the current monotonic time in us */
		timer_reset_value = 0;

		/* Use this as the last timer value we saw */
		lastinc = timer_get_us_down();
	}
}

/*
 * timer without interrupts
 */
unsigned long get_timer(unsigned long base)
{
	unsigned long now = timer_get_us_down();

	/*
	 * Increment the time by the amount elapsed since the last read.
	 * The timer may have wrapped around, but it makes no difference to
	 * our arithmetic here.
	 */
	timer_reset_value += lastinc - now;
	lastinc = now;

	/* Divide by 1000 to convert from us to ms */
	return timer_reset_value / 1000 - base;
}

/* delay x useconds */
void udelay(unsigned usec)
{
	struct mono_time current, end;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_usecs(&end, usec);

	if (mono_time_after(&current, &end)) {
		printk(BIOS_EMERG, "udelay: 0x%08x is impossibly large\n",
				usec);
		/* There's not much we can do if usec is too big. Use a long,
		 * paranoid delay value and hope for the best... */
		end = current;
		mono_time_add_usecs(&end, USECS_PER_SEC);
	}

	while (mono_time_before(&current, &end))
		timer_monotonic_get(&current);
}

