/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

#ifndef ONBOARD_H
#define ONBOARD_H

#define BOARD_LIGHTSENSOR_NAME		"lightsensor"
#define BOARD_LIGHTSENSOR_IRQ		51	/* PIRQT */
#define BOARD_LIGHTSENSOR_I2C_BUS	2	/* I2C1 */
#define BOARD_LIGHTSENSOR_I2C_ADDR	0x44

#define BOARD_TRACKPAD_NAME		"trackpad"
#define BOARD_TRACKPAD_IRQ		37	/* PIRQV */
#define BOARD_TRACKPAD_WAKE_GPIO	13	/* GPIO13 */
#define BOARD_TRACKPAD_I2C_BUS		1	/* I2C0 */
#define BOARD_TRACKPAD_I2C_ADDR		0x67

#define BOARD_TOUCHSCREEN_NAME		"touchscreen"
#define BOARD_TOUCHSCREEN_IRQ		38	/* PIRQW */
#define BOARD_TOUCHSCREEN_WAKE_GPIO	14	/* GPIO14 */
#define BOARD_TOUCHSCREEN_I2C_BUS	2	/* I2C1 */
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x4a

#endif
