/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <stddef.h>
#include <stdint.h>
#include <delay.h>

#include "gpio.h"
#include "pinmux.h"

void __gpio_input(gpio_t gpio, u32 pull)
{
	u32 pinmux_config = PINMUX_INPUT_ENABLE | PINMUX_TRISTATE | pull;

	gpio_set_int_enable(gpio, 0);
	gpio_set_out_enable(gpio, 0);
	gpio_set_mode(gpio, GPIO_MODE_GPIO);
	pinmux_set_config(gpio >> GPIO_PINMUX_SHIFT, pinmux_config);
}

void __gpio_output(gpio_t gpio, int value, u32 od)
{
	gpio_set_int_enable(gpio, 0);
	gpio_set_out_value(gpio, value);
	gpio_set_out_enable(gpio, 1);
	gpio_set_mode(gpio, GPIO_MODE_GPIO);
	pinmux_set_config(gpio >> GPIO_PINMUX_SHIFT, PINMUX_PULL_NONE | od);
}

enum {
	GPIO_GPIOS_PER_PORT = 8,
	GPIO_PORTS_PER_BANK = 4,
	GPIO_BANKS = 8,

	GPIO_GPIOS_PER_BANK = GPIO_GPIOS_PER_PORT * GPIO_PORTS_PER_BANK,
	GPIO_GPIOS = GPIO_BANKS * GPIO_GPIOS_PER_BANK
};

struct gpio_bank {
	// Values
	u32 config[GPIO_PORTS_PER_BANK];
	u32 out_enable[GPIO_PORTS_PER_BANK];
	u32 out_value[GPIO_PORTS_PER_BANK];
	u32 in_value[GPIO_PORTS_PER_BANK];
	u32 int_status[GPIO_PORTS_PER_BANK];
	u32 int_enable[GPIO_PORTS_PER_BANK];
	u32 int_level[GPIO_PORTS_PER_BANK];
	u32 int_clear[GPIO_PORTS_PER_BANK];

	// Masks
	u32 config_mask[GPIO_PORTS_PER_BANK];
	u32 out_enable_mask[GPIO_PORTS_PER_BANK];
	u32 out_value_mask[GPIO_PORTS_PER_BANK];
	u32 in_value_mask[GPIO_PORTS_PER_BANK];
	u32 int_status_mask[GPIO_PORTS_PER_BANK];
	u32 int_enable_mask[GPIO_PORTS_PER_BANK];
	u32 int_level_mask[GPIO_PORTS_PER_BANK];
	u32 int_clear_mask[GPIO_PORTS_PER_BANK];
};

static const struct gpio_bank *gpio_banks = (void *)TEGRA_GPIO_BASE;

static u32 gpio_read_port(int index, size_t offset)
{
	int bank = index / GPIO_GPIOS_PER_BANK;
	int port = (index - bank * GPIO_GPIOS_PER_BANK) / GPIO_GPIOS_PER_PORT;

	return read32((u8 *)&gpio_banks[bank] + offset +
		      port * sizeof(u32));
}

static void gpio_write_port(int index, size_t offset, u32 mask, u32 value)
{
	int bank = index / GPIO_GPIOS_PER_BANK;
	int port = (index - bank * GPIO_GPIOS_PER_BANK) / GPIO_GPIOS_PER_PORT;

	u32 reg = read32((u8 *)&gpio_banks[bank] + offset +
			      port * sizeof(u32));
	u32 new_reg = (reg & ~mask) | (value & mask);

	if (new_reg != reg) {
		write32(new_reg, (u8 *)&gpio_banks[bank] + offset +
			port * sizeof(u32));
	}
}

void gpio_set_mode(gpio_t gpio, enum gpio_mode mode)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, config),
			1 << bit, mode ? (1 << bit) : 0);
}

int gpio_get_mode(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, config));
	return (port & (1 << bit)) != 0;
}

void gpio_set_lock(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT + GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, config),
			1 << bit, 1 << bit);
}

int gpio_get_lock(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT + GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, config));
	return (port & (1 << bit)) != 0;
}

void gpio_set_out_enable(gpio_t gpio, int enable)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, out_enable),
			1 << bit, enable ? (1 << bit) : 0);
}

int gpio_get_out_enable(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, out_enable));
	return (port & (1 << bit)) != 0;
}

void gpio_set_out_value(gpio_t gpio, int value)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, out_value),
			1 << bit, value ? (1 << bit) : 0);
}

int gpio_get_out_value(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, out_value));
	return (port & (1 << bit)) != 0;
}

int gpio_get_in_value(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, in_value));
	return (port & (1 << bit)) != 0;
}

int gpio_get_in_tristate_values(gpio_t gpio[], int num_gpio, int value[])
{
	/*
	 * GPIOs which are tied to stronger external pull up or pull down
	 * will stay there regardless of the internal pull up or pull
	 * down setting.
	 *
	 * GPIOs which are floating will go to whatever level they're
	 * internally pulled to.
	 */

	int temp;
	int index;

	/* Enable internal pull up */
	for (index = 0; index < num_gpio; ++index)
		gpio_input_pullup(gpio[index]);

	/* Wait until signals become stable */
	udelay(10);

	/* Get gpio values at internal pull up */
	for (index = 0; index < num_gpio; ++index)
		value[index] = gpio_get_in_value(gpio[index]);

	/* Enable internal pull down */
	for (index = 0; index < num_gpio; ++index)
		gpio_input_pulldown(gpio[index]);

	/* Wait until signals become stable */
	udelay(10);

	/*
	 * Get gpio values at internal pull down.
	 * Compare with gpio pull up value and then
	 * determine a gpio final value/state:
	 *  0: pull down
	 *  1: pull up
	 *  2: floating
	 */
	for (index = 0; index < num_gpio; ++index) {
		temp = gpio_get_in_value(gpio[index]);
		value[index] = ((value[index] ^ temp) << 1) | temp;
	}

	/* Disable pull up / pull down to conserve power */
	for (index = 0; index < num_gpio; ++index)
		gpio_input(gpio[index]);

	return 0;
}

int gpio_get_int_status(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, int_status));
	return (port & (1 << bit)) != 0;
}

void gpio_set_int_enable(gpio_t gpio, int enable)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, int_enable),
			1 << bit, enable ? (1 << bit) : 0);
}

int gpio_get_int_enable(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, int_enable));
	return (port & (1 << bit)) != 0;
}

void gpio_set_int_level(gpio_t gpio, int high_rise, int edge, int delta)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 value = (high_rise ? (0x000001 << bit) : 0) |
			 (edge ? (0x000100 << bit) : 0) |
			(delta ? (0x010000 << bit) : 0);
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, config),
			0x010101 << bit, value);
}

void gpio_get_int_level(gpio_t gpio, int *high_rise, int *edge, int *delta)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, int_level));
	*high_rise = ((port & (0x000001 << bit)) != 0);
	*edge = ((port & (0x000100 << bit)) != 0);
	*delta = ((port & (0x010000 << bit)) != 0);
}

void gpio_set_int_clear(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
			offsetof(struct gpio_bank, int_clear),
			1 << bit, 1 << bit);
}
