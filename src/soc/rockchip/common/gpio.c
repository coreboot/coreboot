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
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <stdlib.h>

static void gpio_set_dir(gpio_t gpio, enum gpio_dir dir)
{
	clrsetbits_le32(&gpio_port[gpio.port]->swporta_ddr,
			1 << gpio.num, dir << gpio.num);
}

static void gpio_set_pull(gpio_t gpio, enum gpio_pull pull)
{
	u32 pull_val = gpio_get_pull_val(gpio, pull);
	if (is_pmu_gpio(gpio) && IS_ENABLED(CONFIG_SOC_ROCKCHIP_RK3288))
		clrsetbits_le32(gpio_grf_reg(gpio), 3 << (gpio.idx * 2),
				pull_val << (gpio.idx * 2));
	else
		write32(gpio_grf_reg(gpio), RK_CLRSETBITS(3 << (gpio.idx * 2),
			pull_val << (gpio.idx * 2)));
}

void gpio_input(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULLNONE);
	gpio_set_dir(gpio, GPIO_INPUT);
}

void gpio_input_pulldown(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULLDOWN);
	gpio_set_dir(gpio, GPIO_INPUT);
}

void gpio_input_pullup(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULLUP);
	gpio_set_dir(gpio, GPIO_INPUT);
}

void gpio_input_irq(gpio_t gpio, enum gpio_irq_type type, enum gpio_pull pull)
{
	uint32_t int_polarity, inttype_level;
	uint32_t mask = BIT(gpio.num);

	/* gpio pull only PULLNONE, PULLUP, PULLDOWN status */
	assert(pull <= GPIO_PULLDOWN);

	gpio_set_dir(gpio, GPIO_INPUT);
	gpio_set_pull(gpio, pull);

	int_polarity = inttype_level = 0;
	switch (type) {
		case IRQ_TYPE_EDGE_RISING:
			int_polarity = mask;
			inttype_level = mask;
			break;
		case IRQ_TYPE_EDGE_FALLING:
			inttype_level = mask;
			break;
		case IRQ_TYPE_LEVEL_HIGH:
			int_polarity = mask;
			break;
		case IRQ_TYPE_LEVEL_LOW:
			break;
	}
	clrsetbits_le32(&gpio_port[gpio.port]->int_polarity,
			mask, int_polarity);
	clrsetbits_le32(&gpio_port[gpio.port]->inttype_level,
			mask, inttype_level);

	setbits_le32(&gpio_port[gpio.port]->inten, mask);
	clrbits_le32(&gpio_port[gpio.port]->intmask, mask);
}

int gpio_irq_status(gpio_t gpio)
{
	uint32_t mask = BIT(gpio.num);
	uint32_t int_status = read32(&gpio_port[gpio.port]->int_status);

	if (!(int_status & mask))
		return 0;

	setbits_le32(&gpio_port[gpio.port]->porta_eoi, mask);
	return 1;
}

int gpio_get(gpio_t gpio)
{
	return (read32(&gpio_port[gpio.port]->ext_porta) >> gpio.num) & 0x1;
}

void gpio_set(gpio_t gpio, int value)
{
	clrsetbits_le32(&gpio_port[gpio.port]->swporta_dr, 1 << gpio.num,
							   !!value << gpio.num);
}

void gpio_output(gpio_t gpio, int value)
{
	gpio_set(gpio, value);
	gpio_set_dir(gpio, GPIO_OUTPUT);
	gpio_set_pull(gpio, GPIO_PULLNONE);
}
