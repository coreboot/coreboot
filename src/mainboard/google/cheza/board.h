/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __COREBOOT_SRC_MAINBOARD_GOOGLE_CHEZA_BOARD_H
#define __COREBOOT_SRC_MAINBOARD_GOOGLE_CHEZA_BOARD_H

#include <gpio.h>
#include <soc/gpio.h>

#define GPIO_EC_IN_RW	GPIO(11)
#define GPIO_AP_EC_INT	GPIO(122)
#define GPIO_AP_SUSPEND	GPIO(126)
#define GPIO_WP_STATE	GPIO(128)
#define GPIO_H1_AP_INT	GPIO(129)

void setup_chromeos_gpios(void);

#endif  /* ! __COREBOOT_SRC_MAINBOARD_GOOGLE_CHEZA_BOARD_H */
