/*
 * Basic GPIO helpers for Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "gpio.h"

#include <arch/io.h>

static struct a10_gpio *const gpio = (void *)GPIO_BASE;

/**
 * \brief Set a single output pin
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @param[in] pin the pin number in the given port (1 -> 31)
 */
void gpio_set(u8 port, u8 pin)
{
	u32 reg32;

	if ((port > GPS))
		return;

	reg32 = gpio_read(port);
	reg32 |= (1 << pin);
	gpio_write(port, reg32);
}

/**
 * \brief Clear a single output pin
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @param[in] pin the pin number in the given port (1 -> 31)
 */
void gpio_clear(u8 port, u8 pin)
{
	u32 reg32;
	if ((port > GPS))
		return;

	reg32 = gpio_read(port);
	reg32 &= ~(1 << pin);
	gpio_write(port, reg32);
}

/**
 * \brief Get the status of a single input pin
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @param[in] pin the pin number in the given port (1 -> 31)
 * @return 1 if the pin is high, or 0 if the pin is low
 */
int gpio_get(u8 port, u8 pin)
{
	if ((port > GPS))
		return 0;

	return (gpio_read(port) & (1 << pin)) ? 1 : 0;
}

/**
 * \brief Write to a GPIO port
 *
 * Write the state of all output pins in the GPIO port. This only affects pins
 * configured as output pins.
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @param[in] val 32-bit mask indicating which pins to set. For a set bit, the
 *                  corresponding pin will be set. Otherwise, it will be cleared
 */
void gpio_write(u8 port, u32 val)
{
	if ((port > GPS))
		return;

	write32(&gpio->port[port].dat, val);
}

/**
 * \brief Write to a GPIO port
 *
 * Read the state of all input pins in the GPIO port.
 *
 * @param[in] port GPIO port of the pin (GPA -> GPS)
 * @return 32-bit mask indicating which pins are high. For each set bit, the
 *         corresponding pin is high. The value of bits corresponding to pins
 *         which are not configured as inputs is undefined.
 */
u32 gpio_read(u8 port)
{
	if ((port > GPS))
		return 0;

	return read32(&gpio->port[port].dat);
}
