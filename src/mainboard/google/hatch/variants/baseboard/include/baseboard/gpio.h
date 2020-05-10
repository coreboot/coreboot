/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BASEBOARD_GPIO_H
#define BASEBOARD_GPIO_H

#include <soc/gpio.h>

#define GPIO_EC_IN_RW	        GPP_C22

#define GPIO_PCH_WP	        GPP_C20

/* EC wake pin is routed to GPD2/LAN_WAKE# on PCH */
#define GPE_EC_WAKE		GPE0_LAN_WAK

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI

/* EC sync irq is GPP_C13_IRQ */
#define EC_SYNC_IRQ             GPP_C13_IRQ

#endif /* BASEBOARD_GPIO_H */
