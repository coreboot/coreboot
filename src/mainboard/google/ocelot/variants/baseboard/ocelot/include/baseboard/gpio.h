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
#if (CONFIG(BOARD_GOOGLE_OCELOTMCHP) || CONFIG(BOARD_GOOGLE_OCELOTMCHP4ES) \
	|| CONFIG(BOARD_GOOGLE_MODEL_OJAL))
 #define EC_SYNC_IRQ	0 /* Not Connected */
#elif CONFIG(BOARD_GOOGLE_MODEL_KODKOD)
 #define EC_SYNC_IRQ	GPP_F10_IRQ
#elif (CONFIG(BOARD_GOOGLE_OCELOT) || CONFIG(BOARD_GOOGLE_OCELOT4ES) \
	|| CONFIG(BOARD_GOOGLE_OCELOTITE) || CONFIG(BOARD_GOOGLE_OCELOTITE4ES))
 #define EC_SYNC_IRQ	GPP_E08_IRQ
#elif CONFIG(BOARD_GOOGLE_MODEL_MATSU)
 #define EC_SYNC_IRQ	GPP_E08_IRQ
#endif
#define GPIO_PCH_WP	GPP_D02
#define GPIO_SLP_S0_GATE	GPP_C08

#endif /* __BASEBOARD_GPIO_H__ */
