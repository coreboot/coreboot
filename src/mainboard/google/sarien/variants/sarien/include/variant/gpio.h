/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

/* Flash Write Protect */
#define GPIO_PCH_WP		GPP_E15

/* Recovery mode */
#define GPIO_REC_MODE		GPP_E8

const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

struct cros_gpio;
const struct cros_gpio *variant_cros_gpios(size_t *num);

#endif
