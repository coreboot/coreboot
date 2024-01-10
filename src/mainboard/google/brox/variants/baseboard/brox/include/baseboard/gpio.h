/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI
/* EC wake is EC_PCH_INT which is routed to GPP_D0 pin */
#define GPE_EC_WAKE		GPE0_DW1_00
/* WP signal to PCH */
#define GPIO_PCH_WP		GPP_E8
/* Used to gate SoC's SLP_S0# signal */
#define GPIO_SLP_S0_GATE	GPP_D17
/* GPIO IRQ for tight timestamps / wake support */
#define EC_SYNC_IRQ		GPP_D0_IRQ

#endif /* __BASEBOARD_GPIO_H__ */
