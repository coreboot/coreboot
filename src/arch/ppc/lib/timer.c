/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <timer.h>
#include <ppc.h>

unsigned long get_hz(void)
{
	return get_clock_speed();
}

unsigned long ticks_since_boot(void)
{
	extern unsigned long _timebase(void);
	return _timebase();
}

void sleep_ticks(unsigned long ticks)
{
	unsigned long then = ticks + ticks_since_boot();
	while(ticks_since_boot() < then)
		;
}

void udelay(int usecs)
{
	unsigned long ticksperusec = get_hz() / 1000000;

	sleep_ticks(ticksperusec * usecs);
}
