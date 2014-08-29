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

/* Board ID definitions. */
enum {
	BOARD_ID_PROTO_0	= 0,
	BOARD_ID_PROTO_1	= 1,
	BOARD_ID_EVT		= 2,
	BOARD_ID_DVT		= 3,
	BOARD_ID_PVT		= 4,
	BOARD_ID_MP		= 5,
};

enum {
	/* Board ID related GPIOS. */
	BD_ID0		= GPIO(Q3),
	BD_ID1		= GPIO(Q4),
	/* LTE modem related GPIOs */
	MODEM_RESET	= GPIO(S3),
	MODEM_PWR_ON	= GPIO(S4),
	MDM_DET		= GPIO(V1),
	/* Warm reset */
	AP_SYS_RESET_L	= GPIO(I5),
	/* Write Protect */
	SPI_1V8_WP_L	= GPIO(R1),
	WRITE_PROTECT_L	= SPI_1V8_WP_L,
	WRITE_PROTECT_L_INDEX = GPIO_R1_INDEX,
	/* Power Button -- actually active high, but the net names are off. */
	BTN_AP_PWR_L	= GPIO(Q0),
	POWER_BUTTON	= BTN_AP_PWR_L,
	POWER_BUTTON_INDEX = GPIO_Q0_INDEX,
};

#endif /* __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__ */
