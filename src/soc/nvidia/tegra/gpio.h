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

/* Higher level functions for common GPIO configurations. */

void gpio_input(int gpio_index, int pinmux_index);
void gpio_input_pullup(int gpio_index, int pinmux_index);
void gpio_input_pulldown(int gpio_index, int pinmux_index);
void gpio_output(int gpio_index, int pinmux_index, int value);

/* Functions to modify specific GPIO control values. */

enum gpio_mode {
	GPIO_MODE_SPIO = 0,
	GPIO_MODE_GPIO = 1
};
void gpio_set_mode(int gpio_index, enum gpio_mode);
int gpio_get_mode(int gpio_index);

// Lock a GPIO with extreme caution since they can't be unlocked.
void gpio_set_lock(int gpio_index);
int gpio_get_lock(int gpio_index);

void gpio_set_out_enable(int gpio_index, int enable);
int gpio_get_out_enable(int gpio_index);

void gpio_set_out_value(int gpio_index, int value);
int gpio_get_out_value(int gpio_index);

int gpio_get_in_value(int gpio_index);

int gpio_get_int_status(int gpio_index);

void gpio_set_int_enable(int gpio_index, int enable);
int gpio_get_int_enable(int gpio_index);

void gpio_set_int_level(int gpio_index, int high_rise, int edge, int delta);
void gpio_get_int_level(int gpio_index, int *high_rise, int *edge, int *delta);

void gpio_set_int_clear(int gpio_index);

#endif	/* __SOC_NVIDIA_TEGRA_GPIO_H__ */
