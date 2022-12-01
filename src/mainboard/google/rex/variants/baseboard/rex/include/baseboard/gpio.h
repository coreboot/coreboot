/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* Fixme: Update proper GPIO number based on schematics */
/* WP signal to PCH */
#define GPIO_PCH_WP	GPP_H10
/* GPIO IRQ for tight timestamps / wake support */
#define EC_SYNC_IRQ	GPP_A17_IRQ
/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI
/* Used to gate SoC's SLP_S0# signal */
#define GPIO_SLP_S0_GATE	GPP_H14
/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE	GPE0_LAN_WAK
/* Memory configuration board straps */
#define GPIO_MEM_CONFIG_0	GPP_E11
#define GPIO_MEM_CONFIG_1	GPP_E02
#define GPIO_MEM_CONFIG_2	GPP_E01
#define GPIO_MEM_CONFIG_3	GPP_E12
#define GPIO_MEM_CH_SEL		GPP_E13

#endif /* __BASEBOARD_GPIO_H__ */
