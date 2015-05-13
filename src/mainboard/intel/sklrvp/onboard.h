/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef ONBOARD_H
#define ONBOARD_H

/*
 * Gpio based irq for touchpad, 18th index in North Bank
 * MAX_DIRECT_IRQ + GPSW_SIZE + 19
 */
#define SKLRVP_TOUCHPAD_IRQ	33

#define SKLRVP_TOUCH_IRQ	31

#define BOARD_TOUCHPAD_NAME		"touchpad"
#define BOARD_TOUCHPAD_IRQ		SKLRVP_TOUCHPAD_IRQ
#define BOARD_TOUCHPAD_I2C_BUS		0
#define BOARD_TOUCHPAD_I2C_ADDR		0x20

#define BOARD_TOUCHSCREEN_NAME		"touchscreen"
#define BOARD_TOUCHSCREEN_IRQ		SKLRVP_TOUCH_IRQ
#define BOARD_TOUCHSCREEN_I2C_BUS	0
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x4c

#endif
