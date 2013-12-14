/*
 * Helpers to multiplex and configure pins on Allwinner SoCs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "gpio.h"

#include <arch/io.h>

static struct a10_gpio *gpio = (void *)GPIO_BASE;

void gpio_set_func(u8 port, u8 pin, u8 pad_func)
{
	u8 reg, bit;
	u32 reg32;

	if ((port > GPS))
		return;

	pin &= 0x1f;
	reg = pin / 8;
	bit = (pin % 8) * 4;

	reg32 = read32(&gpio->port[port].cfg[reg]);
	reg32 &= ~(0xf << bit);
	reg32 |= (pad_func & 0xf) << bit;
	write32(reg32, &gpio->port[port].cfg[reg]);
}
