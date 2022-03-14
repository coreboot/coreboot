/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI
/* EC wake is EC_SOC_WAKE_ODL which is routed to GPP_F17 */
#define GPE_EC_WAKE		GPE0_DW2_17
/* WP signal to PCH */
#define GPIO_PCH_WP		GPP_E12
/* EC in RW or RO */
#define GPIO_EC_IN_RW		GPP_F18
/* GPIO IRQ for tight timestamps */
#define EC_SYNC_IRQ		GPD2_IRQ
/* GPP_H18 used as dummy here since nissa not selected HAVE_SLP_S0_GATE */
#define GPIO_SLP_S0_GATE	GPP_H18

#endif /* __BASEBOARD_GPIO_H__ */
