/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_ASURADA_GPIO_H__
#define __MAINBOARD_GOOGLE_ASURADA_GPIO_H__

#include <soc/gpio.h>

/* AP_FLASH_WP */
#define GPIO_WP		GPIO(EINT14)
#define GPIO_EC_AP_INT	GPIO(EINT5)
#define GPIO_EC_IN_RW	GPIO(ANT_SEL8)
#define GPIO_H1_AP_INT	GPIO(ANT_SEL9)
/* SD Card Detection */
#define GPIO_SD_CD	GPIO(EINT17)
/* AP_EC_WARM_RST_REQ */
#define GPIO_RESET	GPIO(CAM_PDN2)

void setup_chromeos_gpios(void);

#endif
