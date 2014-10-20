/*
 * Copyright (c) 2011 - 2014 The Linux Foundation. All rights reserved.
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
#include <soc/iomap.h>
#include <soc/ipq_timer.h>
#include <timer.h>

/*
 * DGT runs at 25 MHz / 4, or 6.25 ticks per microsecond
 */
#define DGT_MHZ_NUM	25
#define DGT_MHZ_DEN	4

#define TIMER_TICKS(us) ((DGT_MHZ_NUM*(us) + (DGT_MHZ_DEN - 1)) / DGT_MHZ_DEN)
#define TIMER_USECS(ticks) (DGT_MHZ_DEN*(ticks) / DGT_MHZ_NUM)

/* Clock divider values for the timer. */
#define DGT_CLK_DIV_1	0
#define DGT_CLK_DIV_2	1
#define DGT_CLK_DIV_3	2
#define DGT_CLK_DIV_4	3

/**
 * init_timer - initialize timer
 */
void init_timer(void)
{
	/* disable timer */
	writel_i(0, DGT_ENABLE);

	/* DGT uses TCXO source which is 25MHz.
	 * The timer should run at 1/4th the frequency of TCXO
	 * according to clock plan.
	 * Set clock divider to 4.
	 */
	writel_i(DGT_CLK_DIV_4, DGT_CLK_CTL);

	/* Enable timer */
	writel_i(0, DGT_CLEAR);
	writel_i(DGT_ENABLE_EN, DGT_ENABLE);
}

/**
 * udelay -  generates micro second delay.
 * @param usec: delay duration in microseconds
 */
void udelay(unsigned usec)
{
	uint32_t now;
	uint32_t last;
	uint32_t ticks;
	uint32_t curr_ticks = 0;

	/* Calculate number of ticks required. */
	ticks = TIMER_TICKS(usec);

	/* Obtain the current timer value. */
	last = readl_i(DGT_COUNT_VAL);

	/* Loop until the right number of ticks. */
	while (curr_ticks < ticks) {
		now = readl_i(DGT_COUNT_VAL);
		curr_ticks += now - last;
		last = now;
	}
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, TIMER_USECS(readl_i(DGT_COUNT_VAL)));
}
