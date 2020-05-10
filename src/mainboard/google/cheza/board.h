/* SPDX-License-Identifier: GPL-2.0-only */

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
