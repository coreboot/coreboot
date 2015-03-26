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
 * Foundation, Inc.
 */

#ifndef __SRC_INCLUDE_GPIO_H__
#define __SRC_INCLUDE_GPIO_H__

#include <soc/gpio.h>
#include <types.h>

/* <soc/gpio.h> must typedef a gpio_t that fits in 32 bits. */
_Static_assert(sizeof(gpio_t) <= sizeof(u32), "gpio_t doesn't fit in lb_gpio");

/* The following functions must be implemented by SoC/board code. */
int gpio_get(gpio_t gpio);
void gpio_set(gpio_t gpio, int value);
void gpio_input_pulldown(gpio_t gpio);
void gpio_input_pullup(gpio_t gpio);
void gpio_input(gpio_t gpio);
void gpio_output(gpio_t gpio, int value);

/*
 * Read the value presented by the set of GPIOs, when each pin is interpreted
 * as a base-2 digit (LOW = 0, HIGH = 1).
 *
 * gpio[]: pin positions to read. gpio[0] is less significant than gpio[1].
 * num_gpio: number of pins to read.
 */
int gpio_base2_value(gpio_t gpio[], int num_gpio);

/*
 * Read the value presented by the set of GPIOs, when each pin is interpreted
 * as a base-3 digit (LOW = 0, HIGH = 1, Z/floating = 2).
 * Example: X1 = Z, X2 = 1 -> gpio_base3_value({GPIO(X1), GPIO(X2)}) = 5
 * BASE3() from <base3.h> can generate numbers to compare the result to.
 *
 * gpio[]: pin positions to read. gpio[0] is less significant than gpio[1].
 * num_gpio: number of pins to read.
 */
int gpio_base3_value(gpio_t gpio[], int num_gpio);

#endif /* __SRC_INCLUDE_GPIO_H__ */
