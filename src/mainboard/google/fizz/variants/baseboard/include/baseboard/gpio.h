/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* EC in RW */
#define GPIO_EC_IN_RW		GPP_C6

/* BIOS Flash Write Protect */
#define GPIO_PCH_WP		GPP_C23

/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE		GPE0_LAN_WAK

/* eSPI virtual wire reporting */
#define EC_SCI_GPI		GPE0_ESPI

/* SKU_ID GPIOs */
#define GPIO_SKU_ID0		GPP_C12
#define GPIO_SKU_ID1		GPP_C13
#define GPIO_SKU_ID2		GPP_C14
#define GPIO_SKU_ID3		GPP_C15

/* OEM ID GPIOs */
#define GPIO_OEM_ID1		GPP_D10
#define GPIO_OEM_ID2		GPP_D11
#define GPIO_OEM_ID3		GPP_D12

#endif /* BASEBOARD_GPIO_H */
