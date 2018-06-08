/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MAINBOARD_GPIO_H__
#define __MAINBOARD_GPIO_H__

#include <baseboard/gpio.h>

#undef EN_PP3300_DX_CAM
#define EN_PP3300_DX_CAM	GPP_D8

#undef EN_CAM_PMIC_RST_L
#define EN_CAM_PMIC_RST_L	GPP_D17

#endif /* __MAINBOARD_GPIO_H__ */
