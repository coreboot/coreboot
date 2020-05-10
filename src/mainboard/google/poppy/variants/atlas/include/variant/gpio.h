/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GPIO_H__
#define __MAINBOARD_GPIO_H__

#include <baseboard/gpio.h>

#undef EN_PP3300_DX_CAM
#define EN_PP3300_DX_CAM	GPP_D8

#undef EN_CAM_PMIC_RST_L
#define EN_CAM_PMIC_RST_L	GPP_D17

#define EN_CAM_CLOCK		GPP_D18

#endif /* __MAINBOARD_GPIO_H__ */
