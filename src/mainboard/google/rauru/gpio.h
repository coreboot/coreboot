/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_RAURU_GPIO_H__
#define __MAINBOARD_GOOGLE_RAURU_GPIO_H__

#include <soc/gpio.h>

#define GPIO_XHCI_INIT_DONE	GPIO(EINT28)

void setup_chromeos_gpios(void);

#endif
