/*
 * This file is part of the coreboot project.
 *
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
