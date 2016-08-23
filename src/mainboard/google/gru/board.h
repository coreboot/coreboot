/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
 */

#ifndef __COREBOOT_SRC_MAINBOARD_GOOGLE_GRU_BOARD_H
#define __COREBOOT_SRC_MAINBOARD_GOOGLE_GRU_BOARD_H

#include <gpio.h>

#define GPIO_BACKLIGHT	GPIO(1, C, 1)
#define GPIO_EC_IN_RW	GPIO(3, B, 0)
#define GPIO_EC_IRQ	GPIO(0, A, 1)
#define GPIO_POWEROFF	GPIO(1, A, 6)
#define GPIO_RESET	GPIO(0, B, 3)
#define GPIO_WP		GPIO(1, C, 2)
#define GPIO_P15V_EN	GPIO(0, B, 2)
#define GPIO_P30V_EN	GPIO(0, B, 4)
#define GPIO_P18V_AUDIO_PWREN	GPIO(0, A, 2)

void setup_chromeos_gpios(void);

#endif  /* ! __COREBOOT_SRC_MAINBOARD_GOOGLE_GRU_BOARD_H */
