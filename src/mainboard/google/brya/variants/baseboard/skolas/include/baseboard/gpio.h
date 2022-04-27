/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI
/* EC wake is EC_PCH_INT which is routed to GPP_F17 pin */
#define GPE_EC_WAKE		GPE0_DW2_17
/* WP signal to PCH */
#define GPIO_PCH_WP		GPP_E15
/* EC in RW or RO */
#define GPIO_EC_IN_RW		GPP_F18
/* Used to gate SoC's SLP_S0# signal */
#define GPIO_SLP_S0_GATE	GPP_F9
/* GPIO IRQ for tight timestamps / wake support */
#define EC_SYNC_IRQ		GPP_F17_IRQ

#endif /* __BASEBOARD_GPIO_H__ */
