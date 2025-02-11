/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_SKYWALKER_GPIO_H__
#define __MAINBOARD_GOOGLE_SKYWALKER_GPIO_H__

#include <soc/gpio.h>

#define GPIO_XHCI_INIT_DONE	GPIO(GBE_MDC)
#define GPIO_USB3_HUB_RST_L	GPIO(GPIO07)
#define GPIO_EC_AP_INT_ODL	GPIO(GBE_MDIO)
#define GPIO_AP_EC_WARM_RST_REQ	GPIO(GBE_AUX_PPS0)
#define GPIO_GSC_AP_INT_ODL	GPIO(GBE_AUX_PPS1)

void setup_chromeos_gpios(void);

#endif
