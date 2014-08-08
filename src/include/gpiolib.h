/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#ifndef __SRC_INCLUDE_GPIOLIB_H__
#define __SRC_INCLUDE_GPIOLIB_H__

/* A generic type, use accessor macros to actually access the hardware. */
typedef unsigned gpio_t;

/*
 * Read the value presented by the set of GPIOs, when each pin is interpreted
 * as a number in 0..2 range depending on the external pullup situation.
 *
 * Depending on the third parameter, the return value is either a set of two
 * bit fields, each representing one GPIO value, or a number where each GPIO is
 * included multiplied by 3^gpio_num, resulting in a true tertiary value.
 *
 * gpio[]: pin positions to read. little-endian (less significant value first).
 * num_gpio: number of pins to read.
 * tertiary: 1: pins are interpreted as a quad coded tertiary.
 *           0: pins are interpreted as a set of two bit fields.
 */
int gpio_get_in_tristate_values(gpio_t gpio[], int num_gpio, int tertiary);

/*
 * The following functions are not provided by the common library, but must be
 * implemented by the appropriate SOC/board instead.
 */
int gpio_get_in_value(gpio_t gpio);
void gpio_set_out_value(gpio_t gpio, int value);
void gpio_input_pulldown(gpio_t gpio);
void gpio_input_pullup(gpio_t gpio);
void gpio_input(gpio_t gpio);

#endif
