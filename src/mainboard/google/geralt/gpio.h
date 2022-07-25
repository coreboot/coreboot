/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_GERALT_GPIO_H__
#define __MAINBOARD_GOOGLE_GERALT_GPIO_H__

#include <soc/gpio.h>

#define GPIO_AP_EC_WARM_RST_REQ	GPIO(DPI_HSYNC)

void setup_chromeos_gpios(void);

#endif
