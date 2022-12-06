/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

/* Since this ends up being included in dsdt.asl, <gpio.h> can't be included instead */
#include <soc/gpio.h>

/* SPI Write protect */
#define CROS_WP_GPIO GPIO_67
/* GPIO to identify whether EC is in RO or RW */
#define GPIO_EC_IN_RW GPIO_91

#endif /* __BASEBOARD_GPIO_H__ */
