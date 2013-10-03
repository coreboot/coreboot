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

#include "gpio.h"
#include "pinmux.h"

static void gpio_input_common(int gpio_index, int pinmux_index,
			      uint32_t pconfig)
{
	pconfig |= PINMUX_INPUT_ENABLE;
	gpio_set_int_enable(gpio_index, 0);
	gpio_set_mode(gpio_index, GPIO_MODE_GPIO);
	gpio_set_out_enable(gpio_index, 0);
	pinmux_set_config(pinmux_index, pconfig);
}

void gpio_input(int gpio_index, int pinmux_index)
{
	gpio_input_common(gpio_index, pinmux_index, PINMUX_PULL_NONE);
}

void gpio_input_pullup(int gpio_index, int pinmux_index)
{
	gpio_input_common(gpio_index, pinmux_index, PINMUX_PULL_UP);
}

void gpio_input_pulldown(int gpio_index, int pinmux_index)
{
	gpio_input_common(gpio_index, pinmux_index, PINMUX_PULL_DOWN);
}

void gpio_output(int gpio_index, int pinmux_index, int value)
{
	uint32_t pconfig = PINMUX_PULL_NONE;

	pinmux_set_config(pinmux_index, pconfig | PINMUX_TRISTATE);
	gpio_set_int_enable(gpio_index, 0);
	gpio_set_mode(gpio_index, GPIO_MODE_GPIO);
	gpio_set_out_enable(gpio_index, 1);
	gpio_set_out_value(gpio_index, value);
	pinmux_set_config(pinmux_index, pconfig);
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
	uint32_t config[GPIO_PORTS_PER_BANK];
	uint32_t out_enable[GPIO_PORTS_PER_BANK];
	uint32_t out_value[GPIO_PORTS_PER_BANK];
	uint32_t in_value[GPIO_PORTS_PER_BANK];
	uint32_t int_status[GPIO_PORTS_PER_BANK];
	uint32_t int_enable[GPIO_PORTS_PER_BANK];
	uint32_t int_level[GPIO_PORTS_PER_BANK];
	uint32_t int_clear[GPIO_PORTS_PER_BANK];

	// Masks
	uint32_t config_mask[GPIO_PORTS_PER_BANK];
	uint32_t out_enable_mask[GPIO_PORTS_PER_BANK];
	uint32_t out_value_mask[GPIO_PORTS_PER_BANK];
	uint32_t in_value_mask[GPIO_PORTS_PER_BANK];
	uint32_t int_status_mask[GPIO_PORTS_PER_BANK];
	uint32_t int_enable_mask[GPIO_PORTS_PER_BANK];
	uint32_t int_level_mask[GPIO_PORTS_PER_BANK];
	uint32_t int_clear_mask[GPIO_PORTS_PER_BANK];
};

static const struct gpio_bank *gpio_banks = (void *)TEGRA_GPIO_BASE;

static uint32_t gpio_read_port(int index, size_t offset)
{
	int bank = index / GPIO_GPIOS_PER_BANK;
	int port = (index - bank * GPIO_GPIOS_PER_BANK) / GPIO_GPIOS_PER_PORT;

	return read32((uint8_t *)&gpio_banks[bank] + offset +
		      port * sizeof(uint32_t));
}

static void gpio_write_port(int index, size_t offset,
			    uint32_t mask, uint32_t value)
{
	int bank = index / GPIO_GPIOS_PER_BANK;
	int port = (index - bank * GPIO_GPIOS_PER_BANK) / GPIO_GPIOS_PER_PORT;

	uint32_t reg = read32((uint8_t *)&gpio_banks[bank] + offset +
			      port * sizeof(uint32_t));
	uint32_t new_reg = (reg & ~mask) | (value & mask);

	if (new_reg != reg) {
		write32(new_reg, (uint8_t *)&gpio_banks[bank] + offset +
			port * sizeof(uint32_t));
	}
}

void gpio_set_mode(int gpio_index, enum gpio_mode mode)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, config),
			1 << bit, mode ? (1 << bit) : 0);
}

int gpio_get_mode(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, config));
	return (port & (1 << bit)) != 0;
}

void gpio_set_lock(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT + GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, config),
			1 << bit, 1 << bit);
}

int gpio_get_lock(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT + GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, config));
	return (port & (1 << bit)) != 0;
}

void gpio_set_out_enable(int gpio_index, int enable)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, out_enable),
			1 << bit, enable ? (1 << bit) : 0);
}

int gpio_get_out_enable(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, out_enable));
	return (port & (1 << bit)) != 0;
}

void gpio_set_out_value(int gpio_index, int value)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, out_value),
			1 << bit, value ? (1 << bit) : 0);
}

int gpio_get_out_value(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, out_value));
	return (port & (1 << bit)) != 0;
}

int gpio_get_in_value(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, in_value));
	return (port & (1 << bit)) != 0;
}

int gpio_get_int_status(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, int_status));
	return (port & (1 << bit)) != 0;
}

void gpio_set_int_enable(int gpio_index, int enable)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, int_enable),
			1 << bit, enable ? (1 << bit) : 0);
}

int gpio_get_int_enable(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, int_enable));
	return (port & (1 << bit)) != 0;
}

void gpio_set_int_level(int gpio_index, int high_rise, int edge, int delta)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t value = (high_rise ? (0x000001 << bit) : 0) |
			 (edge ? (0x000100 << bit) : 0) |
			 (delta ? (0x010000 << bit) : 0);
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, config),
			0x010101 << bit, value);
}

void gpio_get_int_level(int gpio_index, int *high_rise, int *edge, int *delta)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	uint32_t port = gpio_read_port(gpio_index,
				       offsetof(struct gpio_bank, int_level));
	*high_rise = ((port & (0x000001 << bit)) != 0);
	*edge = ((port & (0x000100 << bit)) != 0);
	*delta = ((port & (0x010000 << bit)) != 0);
}

void gpio_set_int_clear(int gpio_index)
{
	int bit = gpio_index % GPIO_GPIOS_PER_PORT;
	gpio_write_port(gpio_index, offsetof(struct gpio_bank, int_clear),
			1 << bit, 1 << bit);
}
