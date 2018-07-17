/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 * Copyright 2018-present Facebook, Inc.
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

#ifndef __COREBOOT_SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_GPIO_H
#define __COREBOOT_SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_GPIO_H

#include <types.h>

typedef u32 gpio_t;
#include <gpio.h>

/* The following functions must be implemented by SoC/board code. */


gpio_t gpio_pin_count(void);
void gpio_invert(gpio_t gpio, int value);
int gpio_strap_value(gpio_t gpio);

void gpio_init(void);

#endif
