/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_RAURU_GPIO_H__
#define __MAINBOARD_GOOGLE_RAURU_GPIO_H__

#include <soc/gpio.h>

#define GPIO_BEEP_ON_OD		GPIO(PERIPHERAL_EN1)
#define GPIO_EN_SPKR		GPIO(PERIPHERAL_EN0)
#define GPIO_XHCI_INIT_DONE	GPIO(EINT28)
#define GPIO_FP_RST_1V8_S3_L	GPIO(EINT26)
#define GPIO_AP_FP_FW_UP_STRAP	GPIO(EINT27)
#define GPIO_EN_PWR_FP		GPIO(PERIPHERAL_EN3)

void setup_chromeos_gpios(void);

#endif
