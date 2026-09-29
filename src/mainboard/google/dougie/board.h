/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COREBOOT_SRC_MAINBOARD_GOOGLE_DOUGIE_BOARD_H_
#define _COREBOOT_SRC_MAINBOARD_GOOGLE_DOUGIE_BOARD_H_

#include <gpio.h>

#define GPIO_EC_AP_INT		GPIO(18)
#define GPIO_GSC_AP_INT		GPIO(104)

void setup_chromeos_gpios(void);

#endif /* _COREBOOT_SRC_MAINBOARD_GOOGLE_DOUGIE_BOARD_H_ */
