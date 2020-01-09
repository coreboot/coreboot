/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PCH_GPIO_H
#define PCH_GPIO_H

#include <soc/gpio.h>
#include <intelblocks/gpio_defs.h>

const struct pad_config *get_gpio_table(size_t *num);
const struct pad_config *get_early_gpio_table(size_t *num);

#endif /* PCH_GPIO_H */
