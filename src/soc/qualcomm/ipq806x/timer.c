/*
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
 * Source : APQ8064 LK boot
 *
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <delay.h>
#include <iomap.h>
#include <ipq_timer.h>
#include <timer.h>

#define GPT_FREQ_KHZ    32
#define GPT_FREQ	(GPT_FREQ_KHZ * 1000)	/* 32 KHz */

/**
 * timer_init - initialize timer
 */
void init_timer(void)
{
	writel(0, GPT_ENABLE);
	writel(GPT_ENABLE_EN, GPT_ENABLE);
}

/**
 * udelay -  generates micro second delay.
 * @usec: delay duration in microseconds
 *
 * With 32KHz clock, minimum possible delay is 31.25 Micro seconds and
 * its multiples. In Rumi GPT clock is 32 KHz
 */
void udelay(unsigned usec)
{
	unsigned val;
	unsigned now, last;
	unsigned runcount;

	usec = (usec + GPT_FREQ_KHZ - 1) / GPT_FREQ_KHZ;
	last = readl(GPT_COUNT_VAL);
	runcount = last;
	val = usec + last;

	do {
		now = readl(GPT_COUNT_VAL);
		if (last > now)
			runcount += ((GPT_FREQ - last) + now);
		else
			runcount += (now - last);
		last = now;
	} while (runcount < val);
}

#if 0

/*
 * TODO(vbendeb) clean it up later.
 * Compile out the below code but leave it for now in case it will become
 * necessary later in order to make the platform fully functional.
 */
static unsigned long timestamp;
static unsigned long lastinc;

inline ulong gpt_to_sys_freq(unsigned int gpt)
{
	/*
	 * get_timer() expects the timer increments to be in terms
	 * of CONFIG_SYS_HZ. Convert GPT timer values to CONFIG_SYS_HZ
	 * units.
	 */
	return (((ulong)gpt) / (GPT_FREQ / CONFIG_SYS_HZ));
}

/**
 * get_timer_masked - returns current ticks
 *
 * Returns the current timer ticks, since boot.
 */
ulong get_timer_masked(void)
{
	ulong now = gpt_to_sys_freq(readl(GPT_COUNT_VAL));

	if (lastinc <= now) {	/* normal mode (non roll) */
		/* normal mode */
		timestamp += now - lastinc;
		/* move stamp forward with absolute diff ticks */
	} else {		/* we have overflow of the count down timer */
		timestamp += now + (TIMER_LOAD_VAL - lastinc);
	}

	lastinc = now;

	return timestamp;
}

unsigned long long get_ticks(void)
{
	return readl(GPT_COUNT_VAL);
}

/*
 * Return the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
        return GPT_FREQ;
}
#endif
