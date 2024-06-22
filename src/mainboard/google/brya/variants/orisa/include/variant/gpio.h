/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI
/* EC wake is EC_SOC_WAKE_ODL which is routed to GPP_F17 */
#define GPE_EC_WAKE		GPE0_DW2_17
/* WP signal to PCH */
#define GPIO_PCH_WP		GPP_E3
/* EC in RW or RO */
#define GPIO_EC_IN_RW		GPP_F18
/* GPIO IRQ for tight timestamps, MKBP interrupts */
#define EC_SYNC_IRQ		GPD2_IRQ
/* Used to gate SoC's SLP_S0# signal */
#define GPIO_SLP_S0_GATE	GPP_H18

#endif
