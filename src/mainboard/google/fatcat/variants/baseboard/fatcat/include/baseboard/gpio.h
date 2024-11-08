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
#if CONFIG(BOARD_GOOGLE_FATCATNUVO) || CONFIG(BOARD_GOOGLE_FATCATITE)
 #define EC_SYNC_IRQ	GPP_E07_IRQ
 #define GPIO_PCH_WP	GPP_D02
#elif CONFIG(BOARD_GOOGLE_FATCAT)
 #define EC_SYNC_IRQ	0 /* Not Connected */
 #define GPIO_PCH_WP	GPP_D02
#elif CONFIG(BOARD_GOOGLE_FRANCKA)
 #define EC_SYNC_IRQ	GPP_B05_IRQ
 #define GPIO_PCH_WP	GPP_E22
#endif

#endif /* __BASEBOARD_GPIO_H__ */
