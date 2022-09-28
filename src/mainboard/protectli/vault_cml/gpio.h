/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <stdint.h>
#include <soc/gpio.h>

const struct pad_config *board_gpio_table(size_t *num);

#endif /* __BASEBOARD_GPIO_H__ */
