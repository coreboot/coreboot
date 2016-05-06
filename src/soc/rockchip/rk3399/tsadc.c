/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/tsadc.h>
#include <stdint.h>
#include <stdlib.h>

struct rk3399_tsadc_regs {
	u32	user_con;
	u32	auto_con;
	u32	int_en;
	u32	int_pd;
	u32	reserved0[(0x20 - 0x10) / 4];
	u32	data0;
	u32	data1;
	u32	data2;
	u32	data3;
	u32	comp0_int;
	u32	comp1_int;
	u32	comp2_int;
	u32	comp3_int;
	u32	comp0_shut;
	u32	comp1_shut;
	u32	comp2_shut;
	u32	comp3_shut;
	u32	reserved1[(0x60 - 0x50) / 4];
	u32	hight_int_debounce;
	u32	hight_tshut_debounce;
	u32	auto_period;
	u32	auto_period_ht;
};
check_member(rk3399_tsadc_regs, auto_period_ht, 0x6c);

/* user_con */
#define ADC_POWER_CTRL	(1 << 3)
#define START_MODE	(1 << 4)
#define START_SHIFT	5
#define START_MASK	1
#define INTER_PD_SHIFT	6
#define INTER_PD_MASK	0x3f

/* auto_con */
#define LAST_TSHUT	(1 << 24)
#define SRC3_EN		(1 << 7)
#define SRC2_EN		(1 << 6)
#define SRC1_EN		(1 << 5)
#define SRC0_EN		(1 << 4)
#define Q_SEL		(1 << 1)
#define AUTO_EN		(1 << 0)

/* int_en */
#define TSHUT_CRU_EN_SRC3	(1 << 11)
#define TSHUT_CRU_EN_SRC2	(1 << 10)
#define TSHUT_CRU_EN_SRC1	(1 << 9)
#define TSHUT_CRU_EN_SRC0	(1 << 8)
#define TSHUT_GPIO_EN_SRC3	(1 << 7)
#define TSHUT_GPIO_EN_SRC2	(1 << 6)
#define TSHUT_GPIO_EN_SRC1	(1 << 5)
#define TSHUT_GPIO_EN_SRC0	(1 << 4)

#define AUTO_PERIOD		187500 /* 250ms */
#define AUTO_DEBOUNCE		4
#define AUTO_PERIOD_HT		37500 /* 50ms */
#define AUTO_DEBOUNCE_HT	4
#define TSADC_CLOCK_HZ		(750 * KHz)

/* AD value, correspond to 120 degrees Celsius,
 * Please refer shut value table in:
 * https://patchwork.kernel.org/patch/8908411/
 * A quick ref:
 * {573, 60000}, {599, 75000}, {616, 85000}, {633, 95000},
 * {642, 100000}, {659, 110000}, {677, 120000}, {685, 125000}
 */
#define TSADC_SHUT_VALUE	677

#define GRF_TSADC_TSEN_PD0_ON	RK_SETBITS(0)
#define GRF_TSADC_TSEN_PD0_OFF	RK_CLRBITS(0)
#define GRF_SARADC_TSEN_ON	RK_SETBITS(0)

struct rk3399_tsadc_regs *rk3399_tsadc = (void *)TSADC_BASE;

void tsadc_init(uint32_t polarity)
{
	rkclk_configure_tsadc(TSADC_CLOCK_HZ);

	/* tsadc power sequence */
	clrbits_le32(&rk3399_tsadc->user_con, ADC_POWER_CTRL);
	write32(&rk3399_grf->tsadc_testbit_l, GRF_TSADC_TSEN_PD0_ON);
	udelay(50);
	write32(&rk3399_grf->tsadc_testbit_l, GRF_TSADC_TSEN_PD0_OFF);
	udelay(20);
	write32(&rk3399_grf->saradc_testbit, GRF_SARADC_TSEN_ON);
	udelay(100);

	/* set the tshut polarity */
	write32(&rk3399_tsadc->auto_con, polarity);

	/* setup the automatic mode:
	 * AUTO_PERIOD: interleave between every two accessing of TSADC
	 * AUTO_DEBOUNCE: only generate interrupt or TSHUT when temprature
	 *                is higher than COMP_INT for "debounce" times
	 * AUTO_PERIOD_HT: the interleave between every two accessing after the
	 *                 temperature is higher than COMP_SHUT or COMP_INT
	 * AUTO_DEBOUNCE_HT: only generate interrupt or TSHUT when temperature
	 *                   is higher than COMP_SHUT for "debounce" times.
	 */
	write32(&rk3399_tsadc->auto_period, AUTO_PERIOD);
	write32(&rk3399_tsadc->hight_int_debounce, AUTO_DEBOUNCE);
	write32(&rk3399_tsadc->auto_period_ht, AUTO_PERIOD_HT);
	write32(&rk3399_tsadc->hight_tshut_debounce, AUTO_DEBOUNCE_HT);
	/* Enable the src0, negative temprature coefficient */
	setbits_le32(&rk3399_tsadc->auto_con, Q_SEL | SRC0_EN);
	udelay(100);
	setbits_le32(&rk3399_tsadc->auto_con, AUTO_EN);

	write32(&rk3399_tsadc->comp0_shut, TSADC_SHUT_VALUE);
	write32(&rk3399_tsadc->int_en, TSHUT_CRU_EN_SRC0 | TSHUT_GPIO_EN_SRC0);

	/* Set the tsadc_int pinmux */
	write32(&rk3399_pmugrf->tsadc_int, IOMUX_TSADC_INT);
}
