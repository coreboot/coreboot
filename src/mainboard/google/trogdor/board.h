/* This file is part of the coreboot project. */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COREBOOT_SRC_MAINBOARD_GOOGLE_TROGDOR_BOARD_H_
#define _COREBOOT_SRC_MAINBOARD_GOOGLE_TROGDOR_BOARD_H_

#include <gpio.h>
#include <soc/gpio.h>

#define GPIO_EC_IN_RW	GPIO(118)
#define GPIO_AP_EC_INT	GPIO(94)
#define GPIO_AP_SUSPEND	GPIO(20)
#define GPIO_WP_STATE	GPIO(42)
#define GPIO_H1_AP_INT	GPIO(21)

void setup_chromeos_gpios(void);

#endif /* _COREBOOT_SRC_MAINBOARD_GOOGLE_TROGDOR_BOARD_H_ */
