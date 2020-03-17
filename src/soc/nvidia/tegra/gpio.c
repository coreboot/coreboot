/*
 * This file is part of the coreboot project.
 *
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
#include <gpio.h>
#include <soc/addressmap.h>
#include <stddef.h>
#include <stdint.h>

#include "pinmux.h"

static void __gpio_input(gpio_t gpio, u32 pull)
{
	u32 pinmux_config = PINMUX_INPUT_ENABLE | pull;

	gpio_set_int_enable(gpio, 0);
	gpio_set_out_enable(gpio, 0);
	gpio_set_mode(gpio, GPIO_MODE_GPIO);
	pinmux_set_config(gpio >> GPIO_PINMUX_SHIFT, pinmux_config);
}

static void __gpio_output(gpio_t gpio, int value, u32 od)
{
	gpio_set_int_enable(gpio, 0);
	gpio_set(gpio, value);
	gpio_set_out_enable(gpio, 1);
	gpio_set_mode(gpio, GPIO_MODE_GPIO);
	pinmux_set_config(gpio >> GPIO_PINMUX_SHIFT, PINMUX_PULL_NONE | od);
}

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
		write32((u8 *)&gpio_banks[bank] + offset + port * sizeof(u32),
			new_reg);
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

void gpio_set(gpio_t gpio, int value)
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

int gpio_get(gpio_t gpio)
{
	int bit = gpio % GPIO_GPIOS_PER_PORT;
	u32 port = gpio_read_port(gpio & ((1 << GPIO_PINMUX_SHIFT) - 1),
				  offsetof(struct gpio_bank, in_value));
	return (port & (1 << bit)) != 0;
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

void gpio_input_pulldown(gpio_t gpio)
{
	__gpio_input(gpio, PINMUX_PULL_DOWN);
}

void gpio_input_pullup(gpio_t gpio)
{
	__gpio_input(gpio, PINMUX_PULL_UP);
}

void gpio_input(gpio_t gpio)
{
	__gpio_input(gpio, PINMUX_PULL_NONE);
}

void gpio_output(gpio_t gpio, int value)
{
	__gpio_output(gpio, value, 0);
}

void gpio_output_open_drain(gpio_t gpio, int value)
{
	__gpio_output(gpio, value, PINMUX_OPEN_DRAIN);
}
