/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __MAINBOARD_GPIO_H__
#define __MAINBOARD_GPIO_H__

#include <baseboard/gpio.h>

/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE	GPE0_LAN_WAK

#endif /* __MAINBOARD_GPIO_H__ */
