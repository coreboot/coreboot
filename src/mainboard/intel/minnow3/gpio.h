/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/gpio.h>

const struct pad_config *gpio_table(size_t *num);
const struct pad_config *early_gpio_table(size_t *num);
const struct pad_config *sleep_gpio_table(size_t *num);
