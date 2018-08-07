/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.
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
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <arch/io.h>
#include <arch/lib_helpers.h>
#include <console/console.h>
#include <inttypes.h>
#include <soc/clock.h>
#include <soc/timer.h>
#include <stdint.h>
#include <timer.h>
#include <soc/addressmap.h>
#include <assert.h>

/* Global System Timers Unit (GTI) registers */
struct cn81xx_timer {
	u32 cc_cntcr;
	u32 cc_cntsr;
	u64 cc_cntcv;
	u8 rsvd[0x10];
	u32 cc_cntfid0;
	u32 cc_cntfid1;
	u8 rsvd2[0x98];
	u32 cc_cntrate;
	u32 cc_cntracc;
	u64 cc_cntadd;
	u64 cc_cntmb;
	u64 cc_cntmbts;
	u64 cc_cntmb_int;
	u64 cc_cntmb_int_set;
	u64 cc_cntmb_int_ena_clr;
	u64 cc_cntmb_int_ena_set;
	u64 cc_imp_ctl;
	u8 skip[0x1fef8];
	u32 ctl_cntfrq;
	u32 ctl_cntnsar;
	u32 ctl_cnttidr;
	u8 rsvd3[0x34];
	u32 ctl_cntacr0;
	u8 skip2[0x1ffb8];
	u64 cwd_wdog[48]; /* Offset 0x40000 */
	u8 skip3[0xfe80];
	u64 cwd_poke[48]; /* Offset 0x50000 */
};

check_member(cn81xx_timer, cc_imp_ctl, 0x100);
check_member(cn81xx_timer, ctl_cntacr0, 0x20040);
check_member(cn81xx_timer, cwd_wdog[0], 0x40000);
check_member(cn81xx_timer, cwd_poke[0], 0x50000);


#define GTI_CC_CNTCR_EN			(1 << 0)
#define GTI_CC_CNTCR_HDBG		(1 << 1)
#define GTI_CC_CNTCR_FCREQ		(1 << 8)

#define GTI_CC_CNTSR_DBGH		(1 << 1)
#define GTI_CC_CNTSR_FCACK		(1 << 8)

#define GTI_CWD_WDOG_MODE_SHIFT		0
#define GTI_CWD_WDOG_MODE_MASK		0x3
#define GTI_CWD_WDOG_STATE_SHIFT	2
#define GTI_CWD_WDOG_STATE_MASK		0x3
#define GTI_CWD_WDOG_LEN_SHIFT		4
#define GTI_CWD_WDOG_LEN_MASK		0xffff
#define GTI_CWD_WDOG_CNT_SHIFT		20
#define GTI_CWD_WDOG_CNT_MASK		0xffffff
#define GTI_CWD_WDOC_DSTOP		(1 << 44)
#define GTI_CWD_WDOC_GSTOP		(1 << 45)

static uint64_t timer_raw_value(void)
{
	struct cn81xx_timer *timer = (void *)GTI_PF_BAR0;

	return read64(&timer->cc_cntcv);
}

/**
 * Get GTI counter value.
 * @param mt      Structure to fill
 */
void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value());
}

/* Setup counter to operate at 1MHz */
static const size_t tickrate = 1000000;

/**
 * Init Global System Timers Unit (GTI).
 * Configure timer to run at 1MHz tick-rate.
 */
void init_timer(void)
{
	struct cn81xx_timer *gti = (struct cn81xx_timer *)GTI_PF_BAR0;

	/* Check if the counter was already setup */
	if (gti->cc_cntcr & GTI_CC_CNTCR_EN)
		return;

	u64 sclk = thunderx_get_io_clock();

	/* Use coprocessor clock source */
	write32(&gti->cc_imp_ctl, 0);

	write32(&gti->cc_cntfid0, tickrate);
	write32(&gti->ctl_cntfrq, tickrate);
	write32(&gti->cc_cntrate, ((1ULL << 32) * tickrate) / sclk);

	/* Enable the counter */
	setbits_le32(&gti->cc_cntcr, GTI_CC_CNTCR_EN);

	//u32 u = (CNTPS_CTL_EL1_IMASK | CNTPS_CTL_EL1_EN);
	//BDK_MSR(CNTPS_CTL_EL1, u);
}

void soc_timer_init(void)
{
	raw_write_cntfrq_el0(tickrate);
}

/**
 * Setup the watchdog to expire in timeout_ms milliseconds. When the watchdog
 * expires, the chip three things happen:
 * 1) Expire 1: interrupt that is ignored by the BDK
 * 2) Expire 2: DEL3T interrupt, which is disabled and ignored
 * 3) Expire 3: Soft reset of the chip
 *
 * Since we want a soft reset, we actually program the watchdog to expire at
 * the timeout / 3.
 *
 * @param index      Index of watchdog to configure
 * @param timeout_ms Timeout in milliseconds.
 */
void watchdog_set(const size_t index, unsigned int timeout_ms)
{
	uint64_t sclk = thunderx_get_io_clock();
	uint64_t timeout_sclk = sclk * timeout_ms / 1000;
	struct cn81xx_timer *timer = (struct cn81xx_timer *)GTI_PF_BAR0;

	assert(index < ARRAY_SIZE(timer->cwd_wdog));
	if (index >= ARRAY_SIZE(timer->cwd_wdog))
		return;

	/*
	 * Per comment above, we want the watchdog to expire at 3x the rate
	 * specified
	 */
	timeout_sclk /= 3;
	/* Watchdog counts in 1024 cycle steps */
	uint64_t timeout_wdog = timeout_sclk >> 10;
	/* We can only specify the upper 16 bits of a 24 bit value. Round up */
	timeout_wdog = (timeout_wdog + 0xff) >> 8;
	/* If the timeout overflows the hardware limit, set max */
	if (timeout_wdog >= 0x10000)
		timeout_wdog = 0xffff;

	printk(BIOS_DEBUG, "Watchdog: Set to expire %llu SCLK cycles\n",
	       timeout_wdog << 18);
	clrsetbits_le64(&timer->cwd_wdog[index],
			(GTI_CWD_WDOG_LEN_MASK << GTI_CWD_WDOG_LEN_SHIFT) |
			(GTI_CWD_WDOG_MODE_MASK << GTI_CWD_WDOG_MODE_SHIFT),
			(timeout_wdog << GTI_CWD_WDOG_LEN_SHIFT) |
			(3 << GTI_CWD_WDOG_MODE_SHIFT));
}

/**
 * Signal the watchdog that we are still running.
 *
 * @param index      Index of watchdog to configure.
 */
void watchdog_poke(const size_t index)
{
	struct cn81xx_timer *timer = (struct cn81xx_timer *)GTI_PF_BAR0;

	assert(index < ARRAY_SIZE(timer->cwd_poke));
	if (index >= ARRAY_SIZE(timer->cwd_poke))
		return;

	write64(&timer->cwd_poke[0], 0);
}

/**
 * Disable the hardware watchdog
 *
 * @param index      Index of watchdog to configure.
 */
void watchdog_disable(const size_t index)
{
	struct cn81xx_timer *timer = (struct cn81xx_timer *)GTI_PF_BAR0;

	assert(index < ARRAY_SIZE(timer->cwd_wdog));
	if (index >= ARRAY_SIZE(timer->cwd_wdog))
		return;

	write64(&timer->cwd_wdog[index], 0);
	printk(BIOS_DEBUG, "Watchdog: Disabled\n");
}

/**
 * Return true if the watchdog is configured and running
 *
 * @param index      Index of watchdog to configure.
 *
 * @return           Non-zero if watchdog is running.
 */
int watchdog_is_running(const size_t index)
{
	struct cn81xx_timer *timer = (struct cn81xx_timer *)GTI_PF_BAR0;

	assert(index < ARRAY_SIZE(timer->cwd_wdog));
	if (index >= ARRAY_SIZE(timer->cwd_wdog))
		return 0;

	uint64_t val = read64(&timer->cwd_wdog[index]);

	return !!(val & (GTI_CWD_WDOG_MODE_MASK << GTI_CWD_WDOG_MODE_SHIFT));
}
