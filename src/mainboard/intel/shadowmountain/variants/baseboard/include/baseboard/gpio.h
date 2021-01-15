/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* EC in RW */
#define GPIO_EC_IN_RW		GPP_A8

/* BIOS Flash Write Protect */
#define GPIO_PCH_WP		GPP_B11

/* EC wake is LAN_WAKE# */
#define GPE_EC_WAKE		GPE0_LAN_WAK

/* EC sync IRQ */
#define EC_SYNC_IRQ		GPP_C6_IRQ

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI

#endif /* BASEBOARD_GPIO_H */
