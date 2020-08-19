/* SPDX-License-Identifier: GPL-2.0-only */

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
