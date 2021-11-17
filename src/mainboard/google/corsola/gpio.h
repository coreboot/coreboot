/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_CORSOLA_GPIO_H__
#define __MAINBOARD_GOOGLE_CORSOLA_GPIO_H__

#include <soc/gpio.h>

#define GPIO_EC_AP_INT	GPIO(EINT13)
#define GPIO_WP		GPIO(EINT16)
#define GPIO_BEEP_ON	GPIO(PERIPHERAL_EN4)
#define GPIO_XHCI_DONE  GPIO(PERIPHERAL_EN1)
#define GPIO_EC_IN_RW	GPIO(EINT14)
#define GPIO_GSC_AP_INT	GPIO(EINT15)
#define GPIO_EN_SPK	GPIO(PERIPHERAL_EN3)
#define GPIO_RESET	GPIO(PERIPHERAL_EN0)

void setup_chromeos_gpios(void);

#endif
