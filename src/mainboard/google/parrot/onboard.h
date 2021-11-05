/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PARROT_ONBOARD_H
#define PARROT_ONBOARD_H

#define BOARD_TRACKPAD_NAME         "trackpad"
#define BOARD_TRACKPAD_I2C_ADDR     0x67
#define BOARD_TRACKPAD_IRQ_DVT      16
#define BOARD_TRACKPAD_IRQ_PVT      20
#define BOARD_TRACKPAD_WAKE_GPIO    0x1c

#define GPIO_LID	15

/* GPIO68, active low. For Servo support
 * Treat as active high and let the caller invert if needed. */
#define GPIO_REC_MODE	68


#define GPIO_SPI_WP	70

#endif
