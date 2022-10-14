/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpio.h>

void configure_gpio_pads(void);
void configure_early_gpio_pads(void);

#endif /* __BASEBOARD_GPIO_H__ */
