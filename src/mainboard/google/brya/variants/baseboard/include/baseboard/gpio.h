/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI
/* EC wake is EC_PCH_INT which is routed to GPP_F17 pin */
#define GPE_EC_WAKE	GPE0_DW2_17

#endif /* __BASEBOARD_GPIO_H__ */
