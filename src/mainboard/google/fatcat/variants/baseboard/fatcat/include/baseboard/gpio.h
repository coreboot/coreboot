/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* FIXME: update below code as per board schematics */
/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI
/* GPIO IRQ for tight timestamps / wake support */
#define EC_SYNC_IRQ	0
/* WP signal to PCH */
#define GPIO_PCH_WP	0

#endif /* __BASEBOARD_GPIO_H__ */
