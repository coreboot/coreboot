/* A couple of routines to implement a low-overhead timer for drivers */

 /*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */
#include <cpu/p6/msr.h>
#include <printk.h>
#include "etherboot.h"
#include "timer.h"

void load_timer2(unsigned int ticks)
{
	/* Set up the timer gate, turn off the speaker */
	outb((inb(PPC_PORTB) & ~PPCB_SPKR) | PPCB_T2GATE, PPC_PORTB);
	outb(TIMER2_SEL|WORD_ACCESS|MODE0|BINARY_COUNT, TIMER_MODE_PORT);
	outb(ticks & 0xFF, TIMER2_PORT);
	outb(ticks >> 8, TIMER2_PORT);
}

#define HZ TICKS_PER_SEC
#define CLOCK_TICK_RATE	1193180U /* Underlying HZ */
/* LATCH is used in the interval timer and ftape setup. */
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)	/* For divider */


/* ------ Calibrate the TSC ------- 
 * Return 2^32 * (1 / (TSC clocks per usec)) for do_fast_gettimeoffset().
 * Too much 64-bit arithmetic here to do this cleanly in C, and for
 * accuracy's sake we want to keep the overhead on the CTC speaker (channel 2)
 * output busy loop as low as possible. We avoid reading the CTC registers
 * directly because of the awkward 8-bit access mechanism of the 82C54
 * device.
 */

#define CALIBRATE_LATCH	(5 * LATCH)

static unsigned long long calibrate_tsc(void)
{
	/* Set the Gate high, disable speaker */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	/*
	 * Now let's take care of CTC channel 2
	 *
	 * Set the Gate high, program CTC channel 2 for mode 0,
	 * (interrupt on terminal count mode), binary count,
	 * load 5 * LATCH count, (LSB and MSB) to begin countdown.
	 */
	outb(0xb0, 0x43);			/* binary, mode 0, LSB/MSB, Ch 2 */
	outb(CALIBRATE_LATCH & 0xff, 0x42);	/* LSB of count */
	outb(CALIBRATE_LATCH >> 8, 0x42);	/* MSB of count */

	{
		unsigned long startlow, starthigh;
		unsigned long endlow, endhigh;
		unsigned long count;

		rdtsc(startlow,starthigh);
		count = 0;
		do {
			count++;
		} while ((inb(0x61) & 0x20) == 0);
		rdtsc(endlow,endhigh);

		/* Error: ECTCNEVERSET */
		if (count <= 1)
			goto bad_ctc;

		/* 64-bit subtract - gcc just messes up with long longs */
		__asm__("subl %2,%0\n\t"
			"sbbl %3,%1"
			:"=a" (endlow), "=d" (endhigh)
			:"g" (startlow), "g" (starthigh),
			 "0" (endlow), "1" (endhigh));

		/* Error: ECPUTOOFAST */
		if (endhigh)
			goto bad_ctc;

		endlow /= 5;
		return endlow;
	}

	/*
	 * The CTC wasn't reliable: we got a hit on the very first read,
	 * or the CPU was so fast/slow that the quotient wouldn't fit in
	 * 32 bits..
	 */
bad_ctc:
	printk_err("bad_ctc\n");
	return 0;
}


unsigned long currticks(void)
{
	static unsigned long clocks_per_tick;
	unsigned long clocks_high, clocks_low;
	unsigned long currticks;

	if (!clocks_per_tick) {
		clocks_per_tick = calibrate_tsc();
		printk_info("clocks_per_tick = %d\n", clocks_per_tick);
	}

	/* Read the Time Stamp Counter */
	rdtsc(clocks_low, clocks_high);

	/* currticks = clocks / clocks_per_tick; */
	__asm__("divl %1"
		:"=a" (currticks)
		:"r" (clocks_per_tick), "0" (clocks_low), "d" (clocks_high));


	return currticks;
}
