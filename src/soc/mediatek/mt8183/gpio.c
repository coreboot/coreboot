/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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
#include <gpio.h>

enum {
	EN_OFFSET = 0x60,
	SEL_OFFSET = 0x80,
};

static void gpio_set_pull_pupd(gpio_t gpio, enum pull_enable enable,
			       enum pull_select select)
{
	void *reg = GPIO_TO_IOCFG_BASE(gpio.base) + gpio.offset;
	int bit = gpio.bit;

	if (enable == GPIO_PULL_ENABLE) {
		if (select == GPIO_PULL_DOWN)
			setbits_le32(reg, 1 << (bit + 2));
		else
			clrbits_le32(reg, 1 << (bit + 2));
	}

	if (enable == GPIO_PULL_ENABLE)
		clrsetbits_le32(reg, 3 << bit, 1 << bit);
	else
		clrbits_le32(reg, 3 << bit);
}

static void gpio_set_pull_en_sel(gpio_t gpio, enum pull_enable enable,
				 enum pull_select select)
{
	void *reg = GPIO_TO_IOCFG_BASE(gpio.base) + gpio.offset;
	int bit = gpio.bit;

	if (enable == GPIO_PULL_ENABLE) {
		if (select == GPIO_PULL_DOWN)
			clrbits_le32(reg + SEL_OFFSET, 1 << bit);
		else
			setbits_le32(reg + SEL_OFFSET, 1 << bit);
	}

	if (enable == GPIO_PULL_ENABLE)
		setbits_le32(reg + EN_OFFSET, 1 << bit);
	else
		clrbits_le32(reg + EN_OFFSET, 1 << bit);
}

void gpio_set_pull(gpio_t gpio, enum pull_enable enable,
		   enum pull_select select)
{
	if (gpio.flag)
		gpio_set_pull_pupd(gpio, enable, select);
	else
		gpio_set_pull_en_sel(gpio, enable, select);
}
