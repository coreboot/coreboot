#include <console/console.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <smp/spinlock.h>
#include <delay.h>
#include <thread.h>

#if !defined(__PRE_RAM__)

static unsigned long clocks_per_usec;

#if CONFIG_TSC_CONSTANT_RATE
static unsigned long calibrate_tsc(void)
{
	return tsc_freq_mhz();
}
#else /* CONFIG_TSC_CONSTANT_RATE */
#if !CONFIG_TSC_CALIBRATE_WITH_IO
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
			:"=a" (end.lo), "=d" (end.hi)
			:"g" (start.lo), "g" (start.hi),
			 "0" (end.lo), "1" (end.hi));

		/* Error: ECPUTOOFAST */
		if (end.hi)
			goto bad_ctc;


		/* Error: ECPUTOOSLOW */
		if (end.lo <= CALIBRATE_DIVISOR)
			goto bad_ctc;

		return (end.lo + CALIBRATE_DIVISOR -1)/CALIBRATE_DIVISOR;
	}

	/*
	 * The CTC wasn't reliable: we got a hit on the very first read,
	 * or the CPU was so fast/slow that the quotient wouldn't fit in
	 * 32 bits..
	 */
bad_ctc:
	printk(BIOS_ERR, "bad_ctc\n");
	return 0;
}

#else /*  CONFIG_TSC_CALIBRATE_WITH_IO */

/*
 * this is the "no timer2" version.
 * to calibrate tsc, we get a TSC reading, then do 1,000,000 outbs to port 0x80
 * then we read TSC again, and divide the difference by 1,000,000
 * we have found on a wide range of machines that this gives us a a
 * good microsecond value
 * to +- 10%. On a dual AMD 1.6 Ghz box, it gives us .97 microseconds, and on a
 * 267 Mhz. p5, it gives us 1.1 microseconds.
 * also, since gcc now supports long long, we use that.
 * also no unsigned long long / operator, so we play games.
 * about the only thing you can do with long longs, it seems,
 *is return them and assign them.
 * (and do asm on them, yuck)
 * so avoid all ops on long longs.
 */
static unsigned long long calibrate_tsc(void)
{
	unsigned long long start, end, delta;
	unsigned long result, count;

	printk(BIOS_SPEW, "Calibrating delay loop...\n");
	start = rdtscll();
	// no udivdi3 because we don't like libgcc. (only in x86emu)
	// so we count to 1<< 20 and then right shift 20
	for(count = 0; count < (1<<20); count ++)
		inb(0x80);
	end = rdtscll();

#if 0
	// make delta be (endhigh - starthigh) + (endlow - startlow)
	// but >> 20
	// do it this way to avoid gcc warnings.
	start = tsc_start.hi;
	start <<= 32;
	start |= start.lo;
	end = tsc_end.hi;
	end <<= 32;
	end |= tsc_end.lo;
#endif
	delta = end - start;
	// at this point we have a delta for 1,000,000 outbs. Now rescale for one microsecond.
	delta >>= 20;
	// save this for microsecond timing.
	result = delta;
	printk(BIOS_SPEW, "end %llx, start %llx\n", end, start);
	printk(BIOS_SPEW, "32-bit delta %ld\n", (unsigned long) delta);

	printk(BIOS_SPEW, "%s 32-bit result is %ld\n",
			__func__,
			result);
	return delta;
}


#endif /* CONFIG_TSC_CALIBRATE_WITH_IO */
#endif /* CONFIG_TSC_CONSTANT_RATE */

void init_timer(void)
{
	if (!clocks_per_usec) {
		clocks_per_usec = calibrate_tsc();
		printk(BIOS_INFO, "clocks_per_usec: %lu\n", clocks_per_usec);
	}
}

static inline unsigned long get_clocks_per_usec(void)
{
	init_timer();
	return clocks_per_usec;
}
#else /* !defined(__PRE_RAM__) */
/* romstage calls into cpu/board specific function every time. */
static inline unsigned long get_clocks_per_usec(void)
{
	return tsc_freq_mhz();
}
#endif /* !defined(__PRE_RAM__) */

void udelay(unsigned us)
{
	unsigned long long start;
	unsigned long long current;
	unsigned long long clocks;

	if (!thread_yield_microseconds(us))
		return;

	start = rdtscll();
	clocks = us;
	clocks *= get_clocks_per_usec();
	current = rdtscll();
	while((current - start) < clocks) {
		cpu_relax();
		current = rdtscll();
	}
}

#if CONFIG_TSC_MONOTONIC_TIMER && !defined(__PRE_RAM__) && !defined(__SMM__)
#include <timer.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint64_t last_value;
} mono_counter;

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t current_tick;
	uint64_t ticks_elapsed;

	if (!mono_counter.initialized) {
		init_timer();
		mono_counter.last_value = rdtscll();
		mono_counter.initialized = 1;
	}

	current_tick = rdtscll();
	ticks_elapsed = current_tick - mono_counter.last_value;

	/* Update current time and tick values only if a full tick occurred. */
	if (ticks_elapsed >= clocks_per_usec) {
		uint64_t usecs_elapsed;

		usecs_elapsed = ticks_elapsed / clocks_per_usec;
		mono_time_add_usecs(&mono_counter.time, (long)usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
}
#endif
