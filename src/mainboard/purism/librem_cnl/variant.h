/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <soc/gpe.h>
#include <soc/gpio.h>
#include <soc/romstage.h>

const struct pad_config *variant_gpio_table(size_t *num);

void variant_memory_init_params(FSP_M_CONFIG *mem_cfg);

#endif
