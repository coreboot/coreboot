/*
 * Helpers to multiplex and configure pins on Allwinner SoCs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "gpio.h"

#include <arch/io.h>

static struct a10_gpio *const gpio = (void *)GPIO_BASE;

/**
 * \brief Set the pad function of a single pin
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @param[in] pin the pin number in the given port (1 -> 31)
 * @param[in] pad_func The peripheral function to which to connect this pin
 */
void gpio_set_pin_func(u8 port, u8 pin, u8 pad_func)
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
	write32(&gpio->port[port].cfg[reg], reg32);
}

/**
 * \brief Set the pad function of a group of pins
 *
 * Multiplex a group of pins to the same pad function. This is useful for
 * peripherals that use the same function number for several pins. This function
 * allows those pins to be set with a single call.
 *
 * Example:
 *	gpio_set_multipin_func(GPB, (1 << 23) | (1 << 22), 2);
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @param[in] pin_mask 32-bit mask indicating which pins to re-multiplex. For
 *			each set bit, the corresponding pin will be multiplexed.
 * @param[in] pad_func The peripheral function to which to connect the pins
 */
void gpio_set_multipin_func(u8 port, u32 pin_mask, u8 pad_func)
{
	int j;
	u8 reg, bit;
	u32 reg32, mask_offset;

	if ((port > GPS))
		return;

	for (reg = 0; reg < 4; reg++) {
		mask_offset = 8 * reg;
		/* Don't run the inner loop if we're not touching any pins */
		if (!(pin_mask & (0xff << mask_offset)))
			continue;

		reg32 = read32(&gpio->port[port].cfg[reg]);
		for (j = 0; j < 8; j++) {
			if (!(pin_mask & (1 << (j + mask_offset))))
				continue;
			bit = j * 4;
			reg32 &= ~(0xf << bit);
			reg32 |= (pad_func & 0xf) << bit;
		}
		write32(&gpio->port[port].cfg[reg], reg32);
	}
}
