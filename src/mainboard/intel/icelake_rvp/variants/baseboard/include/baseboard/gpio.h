/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef __BASEBOARD_GPIO_H__
#define __BASEBOARD_GPIO_H__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* eSPI virtual wire reporting */
#define EC_SCI_GPI	GPE0_ESPI

/* EC wake is LAN_WAKE# which is a special DeepSX wake pin */
#define GPE_EC_WAKE	GPE0_LAN_WAK

#endif /* __BASEBOARD_GPIO_H__ */
