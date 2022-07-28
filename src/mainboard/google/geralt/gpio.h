/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_GERALT_GPIO_H__
#define __MAINBOARD_GOOGLE_GERALT_GPIO_H__

#include <soc/gpio.h>

#define GPIO_AP_EC_WARM_RST_REQ	GPIO(DPI_HSYNC)
#define GPIO_AP_WP_ODL		GPIO(GPIO15)
#define GPIO_BEEP_ON_OD		GPIO(I2SIN_WS)
#define GPIO_EC_AP_INT_ODL	GPIO(DPI_DE)
#define GPIO_EN_SPKR		GPIO(I2SIN_D2)
#define GPIO_GSC_AP_INT_ODL	GPIO(GPIO00)
#define GPIO_XHCI_INIT_DONE	GPIO(DPI_CK)

void setup_chromeos_gpios(void);

#endif
