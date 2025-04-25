/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GOOGLE_BLUEY_BOARD_H
#define MAINBOARD_GOOGLE_BLUEY_BOARD_H

#include <assert.h>
#include <gpio.h>

#define GPIO_AP_EC_INT		GPIO(67)
#define GSC_AP_INT(x)		GPIO(x)
#define GPIO_GSC_AP_INT		GSC_AP_INT(CONFIG_MAINBOARD_GPIO_PIN_FOR_GSC_AP_INTERRUPT)

void setup_chromeos_gpios(void);

#endif /* MAINBOARD_GOOGLE_BLUEY_BOARD_H */
