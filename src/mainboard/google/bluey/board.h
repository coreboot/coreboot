/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GOOGLE_BLUEY_BOARD_H
#define MAINBOARD_GOOGLE_BLUEY_BOARD_H

#include <assert.h>
#include <gpio.h>

#define GPIO_AP_EC_INT		GPIO(67)
#define GPIO_GSC_AP_INT		GPIO(34)

void setup_chromeos_gpios(void);

#endif /* MAINBOARD_GOOGLE_BLUEY_BOARD_H */
