/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__
#define __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__

#include <soc/nvidia/tegra132/gpio.h>

/* LTE modem related GPIOs */
enum {
	MODEM_RESET	= GPIO(S3),
	MODEM_PWR_ON	= GPIO(S4),
	MDM_DET		= GPIO(V1),
};

#endif /* __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__ */
