/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LUMPY_ONBOARD_H
#define LUMPY_ONBOARD_H

#define BOARD_LIGHTSENSOR_NAME      "lightsensor"
#define BOARD_LIGHTSENSOR_I2C_ADDR  0x44
#define BOARD_LIGHTSENSOR_IRQ       20

#define BOARD_TRACKPAD_NAME         "trackpad"
#define BOARD_TRACKPAD_I2C_ADDR     0x67
#define BOARD_TRACKPAD_IRQ          21
#define BOARD_TRACKPAD_WAKE_GPIO    0x1b

/* Write Protect: GPIO24 = KBC3_SPI_WP#, active high */
#define GPIO_SPI_WP	24

/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
#define GPIO_REC_MODE	42

#endif
