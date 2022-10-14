/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpio.h>

const struct soc_amd_gpio *early_gpio_table(size_t *size);
const struct soc_amd_gpio *gpio_table(size_t *size);

#endif  /* MAINBOARD_GPIO_H */
