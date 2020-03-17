/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI

/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE	GPE0_LAN_WAK

/* EC sync irq is GPP_C15_IRQ */
#define EC_SYNC_IRQ	GPP_C15_IRQ

/* Memory configuration board straps */
#define GPIO_MEM_CONFIG_0	GPP_C0
#define GPIO_MEM_CONFIG_1	GPP_C3
#define GPIO_MEM_CONFIG_2	GPP_C4
#define GPIO_MEM_CONFIG_3	GPP_C5

#endif /* __BASEBOARD_GPIO_H__ */
