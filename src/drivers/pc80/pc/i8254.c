/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <commonlib/helpers.h>
#include <cpu/x86/tsc.h>
#include <pc80/i8254.h>

/* Initialize i8254 timers */

void setup_i8254(void)
{
	/* Timer 0 (taken from biosemu) */
	outb(TIMER0_SEL | WORD_ACCESS | MODE3 | BINARY_COUNT, TIMER_MODE_PORT);
	outb(0x00, TIMER0_PORT);
	outb(0x00, TIMER0_PORT);

	/* Timer 1 */
	outb(TIMER1_SEL | LOBYTE_ACCESS | MODE3 | BINARY_COUNT,
	     TIMER_MODE_PORT);
	outb(0x12, TIMER1_PORT);
}

#define CLOCK_TICK_RATE	1193180U /* Underlying HZ */

/* ------ Calibrate the TSC -------
 * Too much 64-bit arithmetic here to do this cleanly in C, and for
 * accuracy's sake we want to keep the overhead on the CTC speaker (channel 2)
 * output busy loop as low as possible. We avoid reading the CTC registers
 * directly because of the awkward 8-bit access mechanism of the 82C54
 * device.
 */

#define CALIBRATE_INTERVAL ((2*CLOCK_TICK_RATE)/1000) /* 2ms */
#define CALIBRATE_DIVISOR  (2*1000) /* 2ms / 2000 == 1usec */

unsigned long calibrate_tsc_with_pit(void)
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
	outb(0xb0, 0x43);	/* binary, mode 0, LSB/MSB, Ch 2 */

	outb(CALIBRATE_INTERVAL	& 0xff, 0x42);	/* LSB of count */
	outb(CALIBRATE_INTERVAL	>> 8, 0x42);	/* MSB of count */

	{
		tsc_t start;
		tsc_t end;
		unsigned long count;

		start = rdtsc();
		count = 0;
		do {
			count++;
		} while ((inb(0x61) & 0x20) == 0);
		end = rdtsc();

		/* Error: ECTCNEVERSET */
		if (count <= 1)
			goto bad_ctc;

		/* 64-bit subtract - gcc just messes up with long longs */
		__asm__("subl %2,%0\n\t"
			"sbbl %3,%1"
			: "=a" (end.lo), "=d" (end.hi)
			: "g" (start.lo), "g" (start.hi),
			 "0" (end.lo), "1" (end.hi));

		/* Error: ECPUTOOFAST */
		if (end.hi)
			goto bad_ctc;

		/* Error: ECPUTOOSLOW */
		if (end.lo <= CALIBRATE_DIVISOR)
			goto bad_ctc;

		return DIV_ROUND_UP(end.lo, CALIBRATE_DIVISOR);
	}

	/*
	 * The CTC wasn't reliable: we got a hit on the very first read,
	 * or the CPU was so fast/slow that the quotient wouldn't fit in
	 * 32 bits..
	 */
bad_ctc:
	return 0;
}

#if CONFIG(UNKNOWN_TSC_RATE)
static u32 timer_tsc;

unsigned long tsc_freq_mhz(void)
{
	if (timer_tsc > 0)
		return timer_tsc;

	timer_tsc = calibrate_tsc_with_pit();

	/* Set some semi-ridiculous rate if approximation fails. */
	if (timer_tsc == 0)
		timer_tsc = 5000;

	return timer_tsc;
}
#endif
