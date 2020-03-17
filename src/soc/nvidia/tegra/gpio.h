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

#ifndef __SOC_NVIDIA_TEGRA_GPIO_H__
#define __SOC_NVIDIA_TEGRA_GPIO_H__

#include <stdint.h>

#include "pinmux.h"

typedef u32 gpio_t;

#define GPIO_PINMUX_SHIFT 16
#define GPIO(name) ((gpio_t)(GPIO_##name##_INDEX | \
			     (PINMUX_GPIO_##name << GPIO_PINMUX_SHIFT)))

/* Functions to modify specific GPIO control values. */

enum gpio_mode {
	GPIO_MODE_SPIO = 0,
	GPIO_MODE_GPIO = 1
};
void gpio_set_mode(gpio_t gpio, enum gpio_mode);
int gpio_get_mode(gpio_t gpio);

// Lock a GPIO with extreme caution since they can't be unlocked.
void gpio_set_lock(gpio_t gpio);
int gpio_get_lock(gpio_t gpio);

void gpio_set_out_enable(gpio_t gpio, int enable);
int gpio_get_out_enable(gpio_t gpio);

int gpio_get_out_value(gpio_t gpio);

int gpio_get_int_status(gpio_t gpio);

void gpio_set_int_enable(gpio_t gpio, int enable);
int gpio_get_int_enable(gpio_t gpio);

void gpio_set_int_level(gpio_t gpio, int high_rise, int edge, int delta);
void gpio_get_int_level(gpio_t gpio, int *high_rise, int *edge, int *delta);

void gpio_set_int_clear(gpio_t gpio);

void gpio_output_open_drain(gpio_t gpio, int value);

/* Hardware definitions. */

enum {
	GPIO_GPIOS_PER_PORT = 8,
	GPIO_PORTS_PER_BANK = 4,
	GPIO_BANKS = 8,

	GPIO_GPIOS_PER_BANK = GPIO_GPIOS_PER_PORT * GPIO_PORTS_PER_BANK,
	GPIO_GPIOS = GPIO_BANKS * GPIO_GPIOS_PER_BANK
};

static inline int gpio_index_to_bank(int index)
{
	return index / GPIO_GPIOS_PER_BANK;
}

static inline int gpio_index_to_port(int index)
{
	return (index % GPIO_GPIOS_PER_BANK) / GPIO_GPIOS_PER_PORT;
}

static inline int gpio_to_bit(int index)
{
	return index % GPIO_GPIOS_PER_PORT;
}

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

#endif	/* __SOC_NVIDIA_TEGRA_GPIO_H__ */
