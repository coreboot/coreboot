/*
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* EC in RW */
#define GPIO_EC_IN_RW		GPP_A9

/* BIOS Flash Write Protect */
#define GPIO_PCH_WP		GPP_B11

/* Memory configuration board straps */
#define GPIO_MEM_CONFIG_0	GPP_C12
#define GPIO_MEM_CONFIG_1	GPP_C15
#define GPIO_MEM_CONFIG_2	GPP_C14
#define GPIO_MEM_CONFIG_3	GPP_D15

/* EC wake is LAN_WAKE# */
#define GPE_EC_WAKE		GPE0_LAN_WAK

/* EC sync IRQ */
#define EC_SYNC_IRQ		GPP_C6_IRQ

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI

/* DRAM population strap (value 0=fully-populated, 1=half-populated) */
#define GPIO_MEM_CH_SEL		GPP_A17

#endif /* BASEBOARD_GPIO_H */
