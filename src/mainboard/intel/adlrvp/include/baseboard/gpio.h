/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI

/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE	GPE0_LAN_WAK

#define GPIO_EC_IN_RW	GPP_E7

#endif /* __BASEBOARD_GPIO_H__ */
