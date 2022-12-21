/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

/* Flash Write Protect */
#define GPIO_PCH_WP		GPP_E15

/* Recovery mode */
#define GPIO_REC_MODE		GPP_E8

/* Sensor detection pin */
#define SENSOR_DET_360		GPP_H5

/* DDR channel enable pin */
#define DDR_CH0_EN		GPP_F1
#define DDR_CH1_EN		GPP_F2

/* Memory configuration board straps */
#define GPIO_MEM_CONFIG_0	GPP_F12
#define GPIO_MEM_CONFIG_1	GPP_F13
#define GPIO_MEM_CONFIG_2	GPP_F14
#define GPIO_MEM_CONFIG_3	GPP_F15
#define GPIO_MEM_CONFIG_4	GPP_F16

const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_romstage_gpio_table(size_t *num);

#endif
