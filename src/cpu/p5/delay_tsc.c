#include <printk.h>
#include <arch/io.h>
#include <cpu/p6/msr.h>

static unsigned long clocks_per_usec;


#define CLOCK_TICK_RATE	1193180U /* Underlying HZ */

/* ------ Calibrate the TSC ------- 
 * Too much 64-bit arithmetic here to do this cleanly in C, and for
 * accuracy's sake we want to keep the overhead on the CTC speaker (channel 2)
 * output busy loop as low as possible. We avoid reading the CTC registers
 * directly because of the awkward 8-bit access mechanism of the 82C54
 * device.
 */

#define CALIBRATE_INTERVAL ((20*CLOCK_TICK_RATE)/1000) /* 20ms */
#define CALIBRATE_DIVISOR  (20*1000) /* 20ms / 20000 == 1usec */

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
	outb(CALIBRATE_INTERVAL	& 0xff, 0x42);	/* LSB of count */
	outb(CALIBRATE_INTERVAL	>> 8, 0x42);	/* MSB of count */

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


		/* Error: ECPUTOOSLOW */
		if (endlow <= CALIBRATE_DIVISOR)
			goto bad_ctc;

		return (endlow + CALIBRATE_DIVISOR -1)/CALIBRATE_DIVISOR;
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


void udelay(unsigned long us)
{
        unsigned long long count;
        unsigned long long stop;
        unsigned long long clocks;

	if (!clocks_per_usec) {
		clocks_per_usec = calibrate_tsc();
		printk_info("clocks_per_usec: %u\n", clocks_per_usec);
	}
	clocks = us;
	clocks *= clocks_per_usec;
        rdtscll(count);
        stop = clocks + count;
        while(stop > count) {
                rdtscll(count);
        }
}
