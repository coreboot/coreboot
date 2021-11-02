/* SPDX-License-Identifier: GPL-2.0-only */

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

#endif
