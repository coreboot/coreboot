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

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/tsadc.h>
#include <stdint.h>

struct rk3288_tsadc_regs {
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
check_member(rk3288_tsadc_regs, auto_period_ht, 0x6c);

/* auto_con */
#define LAST_TSHUT	(1 << 24)
#define TSHUT_POL_HIGH	(1 << 8)
#define SRC3_EN		(1 << 7)
#define SRC2_EN		(1 << 6)
#define SRC1_EN		(1 << 5)
#define SRC0_EN		(1 << 4)
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

#define AUTO_PERIOD	10
#define AUTO_DEBOUNCE	4
#define AUTO_PERIOD_HT	10
#define AUTO_DEBOUNCE_HT	4
#define TSADC_CLOCK_HZ		(8 * KHz)

/* AD value, correspond to 120 degrees Celsius */
#define TSADC_SHUT_VALUE	3437

struct rk3288_tsadc_regs *rk3288_tsadc = (void *)TSADC_BASE;

void tsadc_init(void)
{
	rkclk_configure_tsadc(TSADC_CLOCK_HZ);

	setbits32(&rk3288_tsadc->auto_con, LAST_TSHUT);

	setbits32(&rk3288_tsadc->int_en,
			TSHUT_CRU_EN_SRC2 | TSHUT_CRU_EN_SRC1 |
			TSHUT_GPIO_EN_SRC2 | TSHUT_GPIO_EN_SRC1);

	write32(&rk3288_tsadc->auto_period, AUTO_PERIOD);
	write32(&rk3288_tsadc->hight_int_debounce, AUTO_DEBOUNCE);
	write32(&rk3288_tsadc->auto_period_ht, AUTO_PERIOD_HT);
	write32(&rk3288_tsadc->hight_tshut_debounce, AUTO_DEBOUNCE_HT);

	write32(&rk3288_tsadc->comp1_shut, TSADC_SHUT_VALUE);
	write32(&rk3288_tsadc->comp2_shut, TSADC_SHUT_VALUE);

	/* polarity set to high,channel1 for cpu,channel2 for gpu */
	setbits32(&rk3288_tsadc->auto_con, TSHUT_POL_HIGH | SRC2_EN |
				SRC1_EN | AUTO_EN);

	/*
	  tsadc iomux must be set after the tshut polarity setting,
	  since the tshut polarity default low active,
	  so if you enable tsadc iomux,it will output high
	 */
	setbits32(&rk3288_pmu->iomux_tsadc_int, IOMUX_TSADC_INT);
}
