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

#ifndef __SOC_NVIDIA_TEGRA_GPIO_H__
#define __SOC_NVIDIA_TEGRA_GPIO_H__

#include <stdint.h>
#include <gpiolib.h>

#include "pinmux.h"

#define GPIO_PINMUX_SHIFT 16
#define GPIO(name) ((gpio_t)(GPIO_##name##_INDEX | \
			     (PINMUX_GPIO_##name << GPIO_PINMUX_SHIFT)))

void __gpio_output(gpio_t gpio, int value, u32 open_drain);
void __gpio_input(gpio_t gpio, u32 pull);

/* Higher level function wrappers for common GPIO configurations. */

static inline void gpio_output(gpio_t gpio, int value)
{
	__gpio_output(gpio, value, 0);
}

static inline void gpio_output_open_drain(gpio_t gpio, int value)
{
	__gpio_output(gpio, value, PINMUX_OPEN_DRAIN);
}

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

#endif	/* __SOC_NVIDIA_TEGRA_GPIO_H__ */
