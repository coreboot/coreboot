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
#include <console/console.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <stdlib.h>

static void __gpio_input(gpio_t gpio, u32 pull)
{
	u32 pull_val = gpio_get_pull_val(gpio, pull);
	clrbits_le32(&gpio_port[gpio.port]->swporta_ddr, 1 << gpio.num);
	if (is_pmu_gpio(gpio))
		clrsetbits_le32(gpio_grf_reg(gpio), 3 << (gpio.idx * 2),
				pull_val << (gpio.idx * 2));
	else
		write32(gpio_grf_reg(gpio), RK_CLRSETBITS(3 << (gpio.idx * 2),
			pull_val << (gpio.idx * 2)));
}

void gpio_input(gpio_t gpio)
{
	__gpio_input(gpio, PULLNONE);
}

void gpio_input_pulldown(gpio_t gpio)
{
	__gpio_input(gpio, PULLDOWN);
}

void gpio_input_pullup(gpio_t gpio)
{
	__gpio_input(gpio, PULLUP);
}

int gpio_get(gpio_t gpio)
{
	return (read32(&gpio_port[gpio.port]->ext_porta) >> gpio.num) & 0x1;
}

void gpio_output(gpio_t gpio, int value)
{
	setbits_le32(&gpio_port[gpio.port]->swporta_ddr, 1 << gpio.num);
	clrsetbits_le32(&gpio_port[gpio.port]->swporta_dr, 1 << gpio.num,
							   !!value << gpio.num);
}
