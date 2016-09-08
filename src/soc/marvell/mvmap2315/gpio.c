/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/gpio.h>
#include <soc/pinmux.h>

struct mvmap2315_gpio_regs *gpio_banks[] = {
	(struct mvmap2315_gpio_regs *)MVMAP2315_GPIOF_BASE,
	(struct mvmap2315_gpio_regs *)MVMAP2315_GPIOG_BASE,
	(struct mvmap2315_gpio_regs *)MVMAP2315_GPIOH_BASE,
};

int gpio_get(gpio_t gpio)
{
	return (read32(&gpio_banks[gpio.bank]->plr) >> gpio.idx) & 0x1;
}

void gpio_set(gpio_t gpio, int value)
{
	if (value)
		setbits_le32(&gpio_banks[gpio.bank]->psr, 1 << gpio.idx);
	else
		setbits_le32(&gpio_banks[gpio.bank]->pcr, 1 << gpio.idx);
}

void gpio_input_pulldown(gpio_t gpio)
{
	set_pinmux(PINMUX(GET_GPIO_PAD(gpio), 0, 1, 0, 0, PULLDOWN));
	clrbits_le32(&gpio_banks[gpio.bank]->pdr, 1 << gpio.idx);
}

void gpio_input_pullup(gpio_t gpio)
{
	set_pinmux(PINMUX(GET_GPIO_PAD(gpio), 0, 1, 0, 0, PULLUP));
	clrbits_le32(&gpio_banks[gpio.bank]->pdr, 1 << gpio.idx);
}

void gpio_input(gpio_t gpio)
{
	set_pinmux(PINMUX(GET_GPIO_PAD(gpio), 0, 1, 0, 0, PULLNONE));
	clrbits_le32(&gpio_banks[gpio.bank]->pdr, 1 << gpio.idx);
}

void gpio_output(gpio_t gpio, int value)
{
	setbits_le32(&gpio_banks[gpio.bank]->pdr, 1 << gpio.idx);

	if (value)
		setbits_le32(&gpio_banks[gpio.bank]->psr, 1 << gpio.idx);
	else
		setbits_le32(&gpio_banks[gpio.bank]->pcr, 1 << gpio.idx);
}
