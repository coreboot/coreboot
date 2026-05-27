/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __MAINBOARD_GPIO_H__
#define __MAINBOARD_GPIO_H__

#include <baseboard/gpio.h>

#define GPIO_PCH_WP GPP_D02
/* eSPI virtual wire reporting */
#define EC_SCI_GPI GPE0_ESPI
/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE GPE0_LAN_WAK
#define EC_SYNC_IRQ GPP_F14_IRQ

#endif /* __MAINBOARD_GPIO_H__ */
