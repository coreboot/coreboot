/* SPDX-License-Identifier: BSD-3-Clause */

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
