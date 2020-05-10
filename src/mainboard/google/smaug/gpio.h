/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_SMAUG_GPIO_H__
#define __MAINBOARD_GOOGLE_SMAUG_GPIO_H__

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
	BOARD_ID_EVT		= BOARD_REV2,
	BOARD_ID_DVT		= BOARD_REV3,
	BOARD_ID_PVT		= BOARD_REV4,
	BOARD_ID_MP		= BOARD_REV5,
};

enum {
	/* Board ID related GPIOS. */
	BD_ID0			= GPIO(K0),
	BD_ID1			= GPIO(K1),

	/* Warm reset */
	AP_SYS_RESET_L		= GPIO(M5),

	/* Write Protect */
	SPI_1V8_WP_L		= GPIO(K2),
	WRITE_PROTECT_L	= SPI_1V8_WP_L,

	/* Power button */
	BTN_AP_PWR_L		= GPIO(X5),
	POWER_BUTTON		= BTN_AP_PWR_L,

	/* EC in RW signal */
	EC_IN_RW		= GPIO(E3),

	/* Panel related GPIOs */
	LCD_EN			= GPIO(V1),
	LCD_RST_L		= GPIO(V2),
	EN_VDD18_LCD		= GPIO(V3),
	EN_VDD_LCD		= GPIO(V4),
};

#endif /* __MAINBOARD_GOOGLE_SMAUG_GPIO_H__ */
