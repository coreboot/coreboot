/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GPIO_H__
#define __MAINBOARD_GPIO_H__

#include <baseboard/gpio.h>

/* Nautilus differs from baseboard in the GPIO used for EN_PP3300_DX_CAM. */
#undef EN_PP3300_DX_CAM
#define EN_PP3300_DX_CAM	GPP_C8

#endif /* __MAINBOARD_GPIO_H__ */
