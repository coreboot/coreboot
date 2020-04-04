/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <assert.h>
#include <delay.h>
#include <soc/clock.h>
#include <soc/saradc.h>
#include <stdint.h>
#include <timer.h>

struct rk3399_saradc_regs {
	u32 data;
	u32 stas;
	u32 ctrl;
	u32 dly_pu_soc;
};
check_member(rk3399_saradc_regs, dly_pu_soc, 0xc);

struct rk3399_saradc_regs *rk3399_saradc = (void *)SARADC_BASE;

/* SARADC_STAS: conversion done */
#define ADC_STOP	0

/* SARADC_CTRL */
#define INT_EN			(1 << 5)
#define ADC_PWR_CTRL		(1 << 3)
#define ADC_CHN_SEL_MASK	7
#define ADC_CHN_SEL_SHIFT	0

/* SARADC_DATA, 10[0:9] bits */
#define DATA_MASK		0x3FF

#define SARADC_HZ		(4*MHz)

#define SARADC_MAX_CHANNEL	6

#define SARADC_DELAY_PU                (1 * 1000 * 1000 * 1000 / SARADC_HZ * 4)

u32 get_saradc_value(u32 chn)
{
	u32 adc_value;
	struct stopwatch sw;

	assert(chn < SARADC_MAX_CHANNEL);
	rkclk_configure_saradc(SARADC_HZ);

	/* power down adc converter */
	clrbits32(&rk3399_saradc->ctrl, ADC_PWR_CTRL);

	/* select channel */
	clrsetbits32(&rk3399_saradc->ctrl,
		     ADC_CHN_SEL_MASK << ADC_CHN_SEL_SHIFT,
		     chn << ADC_CHN_SEL_SHIFT);

	/* power up */
	setbits32(&rk3399_saradc->ctrl, ADC_PWR_CTRL);

	udelay(SARADC_DELAY_PU);

	stopwatch_init_msecs_expire(&sw, 10);
	do {
		if (read32(&rk3399_saradc->stas) == ADC_STOP) {
			adc_value = read32(&rk3399_saradc->data) & DATA_MASK;
			return adc_value;
		}
	} while (!stopwatch_expired(&sw));

	return -1;
}
