/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

/* Flash Write Protect */
#define GPIO_PCH_WP		GPP_C22

/* Recovery mode */
#define GPIO_REC_MODE		GPP_A23

/* DDR channel enable pin */
#define DDR_CHA_EN		GPP_H4
#define DDR_CHB_EN		GPP_H5

/* Memory configuration board straps */
#define GPIO_MEM_CONFIG_0	GPP_F11
#define GPIO_MEM_CONFIG_1	GPP_F12
#define GPIO_MEM_CONFIG_2	GPP_F13
#define GPIO_MEM_CONFIG_3	GPP_F14
#define GPIO_MEM_CONFIG_4	GPP_F15

/* DQ Memory Interleaved */
#define MEMORY_INTERLEAVED	GPP_E3

const struct pad_config *override_gpio_table(size_t *num);
const struct pad_config *override_early_gpio_table(size_t *num);

#endif
