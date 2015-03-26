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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/soc.h>
#include <stdlib.h>

struct rk3288_gpio_regs *gpio_port[] = {
	(struct rk3288_gpio_regs *)0xff750000,
	(struct rk3288_gpio_regs *)0xff780000,
	(struct rk3288_gpio_regs *)0xff790000,
	(struct rk3288_gpio_regs *)0xff7a0000,
	(struct rk3288_gpio_regs *)0xff7b0000,
	(struct rk3288_gpio_regs *)0xff7c0000,
	(struct rk3288_gpio_regs *)0xff7d0000,
	(struct rk3288_gpio_regs *)0xff7e0000,
	(struct rk3288_gpio_regs *)0xff7f0000
};

enum {
	PULLNONE = 0,
	PULLUP,
	PULLDOWN
};

#define PMU_GPIO_PORT 0

static void __gpio_input(gpio_t gpio, u32 pull)
{
	clrbits_le32(&gpio_port[gpio.port]->swporta_ddr, 1 << gpio.num);
	if (gpio.port == PMU_GPIO_PORT)
		clrsetbits_le32(&rk3288_pmu->gpio0pull[gpio.bank],
				3 << (gpio.idx * 2),  pull << (gpio.idx * 2));
	else
		write32(&rk3288_grf->gpio1_p[(gpio.port - 1)][gpio.bank],
			RK_CLRSETBITS(3 << (gpio.idx * 2),
				   pull << (gpio.idx * 2)));
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
