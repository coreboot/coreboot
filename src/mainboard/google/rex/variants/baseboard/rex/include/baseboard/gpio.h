/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* GPIO IRQ for tight timestamps / wake support */
#define EC_SYNC_IRQ	0
/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI
/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE	GPE0_LAN_WAK

#endif /* __BASEBOARD_GPIO_H__ */
