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
 */

#ifndef __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__
#define __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__

#include <gpio.h>
#include <base3.h>

/* Board ID definitions. */
enum {
	BOARD_REV0 = BASE3(0, 0),
	BOARD_REV1 = BASE3(0, 1),
	BOARD_REV2 = BASE3(0, Z),
	BOARD_REV3 = BASE3(1, 0),
	BOARD_REV4 = BASE3(1, 1),
	BOARD_REV5 = BASE3(1, Z),
	BOARD_REV6 = BASE3(Z, 0),
	BOARD_REV7 = BASE3(Z, 1),
	BOARD_REV8 = BASE3(Z, Z),

	BOARD_ID_PROTO_0	= BOARD_REV0,
	BOARD_ID_PROTO_1	= BOARD_REV1,
	BOARD_ID_PROTO_3	= BOARD_REV2,
	BOARD_ID_PROTO_4	= BOARD_REV3,
	BOARD_ID_EVT		= BOARD_REV4,
	BOARD_ID_DVT		= BOARD_REV5,
	BOARD_ID_PVT		= BOARD_REV6,
	BOARD_ID_MP		= BOARD_REV7,
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
	/* Power button - Depending on board id, maybe active high / low */
	BTN_AP_PWR	= GPIO(Q0),
	POWER_BUTTON	= BTN_AP_PWR,
	/* EC in RW signal */
	EC_IN_RW	= GPIO(U4),

	/* Panel related GPIOs */
	LCD_EN		= GPIO(H5),
	LCD_RST_L	= GPIO(H3),
	EN_VDD18_LCD	= GPIO(X0),
	EN_VDD_LCD	= GPIO(BB6),	/* P1/P3 board */
};

#endif /* __MAINBOARD_GOOGLE_RUSH_RYU_GPIO_H__ */
