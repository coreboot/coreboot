/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef ONBOARD_H
#define ONBOARD_H

#define BOARD_TRACKPAD_NAME		"trackpad"
#define BOARD_TRACKPAD_IRQ		37	/* PIRQV */
#define BOARD_TRACKPAD_WAKE_GPIO	12	/* GPIO12 */
#define BOARD_TRACKPAD_I2C_BUS		1	/* I2C0 */
#define BOARD_TRACKPAD_I2C_ADDR		0x2C

#define BOARD_TOUCHSCREEN_NAME		"touchscreen"
#define BOARD_TOUCHSCREEN_IRQ		38	/* PIRQW */
#define BOARD_TOUCHSCREEN_WAKE_GPIO	25	/* GPIO25 */
#define BOARD_TOUCHSCREEN_I2C_BUS	2	/* I2C1 */
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x10

#define BOARD_WLAN_WAKE_GPIO		10	/* GPIO10 */
#define BOARD_PP3300_CODEC_GPIO		45	/* GPIO45 */
#define BOARD_WLAN_DISABLE_GPIO		46	/* GPIO46 */

#endif
