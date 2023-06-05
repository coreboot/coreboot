/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _BD_EGS_GPIO_
#define _BD_EGS_GPIO_

#include <gpio.h>

const struct pad_config *get_gpio_table(size_t *num);

const struct pad_config *get_override_fsp_gpio_table(size_t *num);

#endif /* _BD_EGS_GPIO_ */
