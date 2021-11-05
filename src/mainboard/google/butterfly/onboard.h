/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BUTTERFLY_ONBOARD_H
#define BUTTERFLY_ONBOARD_H

/* uses 7-bit I2C address */
/* must be set to edge triggered */
#define BOARD_TRACKPAD_NAME         "trackpad"
#define BOARD_TRACKPAD_I2C_ADDR     0x67
#define BOARD_TRACKPAD_IRQ          22		/* PIRQG - 22 Edge triggered */
#define BOARD_TRACKPAD_WAKE_GPIO	0x1b

/* defines for programming the MAC address */
#define BUTTERFLY_NIC_VENDOR_ID		0x10EC
#define BUTTERFLY_NIC_DEVICE_ID		0x8136

/* 0x00: White LINK LED and Amber ACTIVE LED */
#define BUTTERFLY_NIC_LED_MODE		0x00

/* SPI write protect, active low */
#define WP_GPIO		6

#endif
