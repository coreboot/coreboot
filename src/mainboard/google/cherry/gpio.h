/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_CHERRY_GPIO_H__
#define __MAINBOARD_GOOGLE_CHERRY_GPIO_H__

#include <soc/gpio.h>

#define GPIO_EC_AP_INT	GPIO(GPIO_04)
#define GPIO_WP		GPIO(GPIO_05)
#define GPIO_SD_CD	GPIO(I2SO1_D1)
#define GPIO_BEEP_ON	GPIO(DMIC1_DAT)
#define GPIO_XHCI_DONE  GPIO(DGI_D4)
#define GPIO_EC_IN_RW	GPIO(DGI_D10)
#define GPIO_GSC_AP_INT	GPIO(DGI_D11)
#define GPIO_RST_RT1011	GPIO(DGI_VSYNC)
#define GPIO_EN_SPK	GPIO(UART1_RTS)
#define GPIO_RESET	GPIO(UART1_CTS)

void setup_chromeos_gpios(void);

#endif
