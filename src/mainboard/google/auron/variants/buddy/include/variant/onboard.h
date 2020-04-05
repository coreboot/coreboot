/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef ONBOARD_H
#define ONBOARD_H

/* defines for programming the MAC address */
#define BUDDY_NIC_VENDOR_ID		0x10EC
#define BUDDY_NIC_DEVICE_ID		0x8168

/* 0x00: White LINK LED and Amber ACTIVE LED */
#define BUDDY_NIC_LED_MODE		0x00

#define BOARD_TOUCHSCREEN_NAME		"touchscreen"
#define BOARD_TOUCHSCREEN_IRQ		38	/* PIRQW */
#define BOARD_TOUCHSCREEN_WAKE_GPIO	25	/* GPIO25 */
#define BOARD_TOUCHSCREEN_I2C_BUS	2	/* I2C1 */
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x10

/* NIC wake is GPIO 8 */
#define BUDDY_NIC_WAKE_GPIO		8

/* WLAN wake is GPIO 10 */
#define BOARD_WLAN_WAKE_GPIO		10
#define BOARD_PP3300_CODEC_GPIO		45	/* GPIO45 */
#define BOARD_WLAN_DISABLE_GPIO		46	/* GPIO46 */

#endif
