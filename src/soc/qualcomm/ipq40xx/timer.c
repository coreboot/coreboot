/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <device/mmio.h>
#include <delay.h>
#include <soc/iomap.h>
#include <soc/ipq_timer.h>
#include <timer.h>

#define GCNT_FREQ_MHZ		48

#define TIMER_TICKS(us)		(GCNT_FREQ_MHZ * (us))
#define TIMER_USECS(ticks)	((ticks) / GCNT_FREQ_MHZ)

/**
 * init_timer - initialize timer
 */
void init_timer(void)
{
	/* disable timer */
	write32(GCNT_CNTCR, 0);

	/* Reset the counters to zero */
	write32(GCNT_GLB_CNTCV_LO, 0);
	write32(GCNT_GLB_CNTCV_HI, 0);

	/* Enable timer */
	write32(GCNT_CNTCR, 1);
}

static inline uint64_t read_gcnt_val(void)
{
	uint32_t hi, lo;

	do {
		hi = read32(GCNT_CNTCV_HI);
		lo = read32(GCNT_CNTCV_LO);
	} while (hi != read32(GCNT_CNTCV_HI));

	return ((((uint64_t)hi) << 32) | lo);
}

/**
 * udelay -  generates micro second delay.
 * @param usec: delay duration in microseconds
 */
void udelay(unsigned int usec)
{
	uint64_t expire;

	expire = read_gcnt_val() + TIMER_TICKS(usec);

	while (expire >= read_gcnt_val())
		;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, TIMER_USECS(read_gcnt_val()));
}
