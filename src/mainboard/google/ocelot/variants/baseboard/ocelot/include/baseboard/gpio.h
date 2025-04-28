/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI
/*
 * EC_SYNC_IRQ - GPIO IRQ for tight timestamps / wake support
 * GPIO_PCH_WP - WP signal to PCH
 */
//TODO for Nuvo: #define EC_SYNC_IRQ	GPP_E07_IRQ
#if CONFIG(BOARD_GOOGLE_OCELOT)
 #define EC_SYNC_IRQ	0 /* Not Connected */
#endif
#define GPIO_PCH_WP	GPP_D02
#define GPIO_SLP_S0_GATE	0 /* Not Connected */

#endif /* __BASEBOARD_GPIO_H__ */
