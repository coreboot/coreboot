/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

const struct pad_config *variant_gpio_table(size_t *num);

#endif
