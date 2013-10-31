/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#define BOARD_PIRQ_INTERRUPT		0
#define BOARD_GPIO_INTERRUPT		1
#define BOARD_GPIO_OFFSET		162
#define GPIO_INTERRUPT(x)		(BOARD_GPIO_OFFSET + (x))

#define BOARD_TRACKPAD_NAME		"trackpad"
#define BOARD_TRACKPAD_IRQ		27	/* PIRQL */
#define BOARD_TRACKPAD_IRQ_TYPE		BOARD_PIRQ_INTERRUPT
#define BOARD_TRACKPAD_WAKE_GPIO	13	/* GPIO13 */
#define BOARD_TRACKPAD_I2C_BUS		1	/* I2C0 */
#define BOARD_TRACKPAD_I2C_ADDR		0x4b

#define BOARD_TOUCHSCREEN_NAME		"touchscreen"
#define BOARD_TOUCHSCREEN_IRQ		28	/* PIRQM */
#define BOARD_TOUCHSCREEN_IRQ_TYPE	BOARD_PIRQ_INTERRUPT
#define BOARD_TOUCHSCREEN_WAKE_GPIO	14	/* GPIO14 */
#define BOARD_TOUCHSCREEN_I2C_BUS	2	/* I2C1 */
#define BOARD_TOUCHSCREEN_I2C_ADDR	0x4b

#define BOARD_CODEC_NAME		"codec"
#define BOARD_CODEC_IRQ			30	/* PIRQO */
#define BOARD_CODEC_IRQ_TYPE		BOARD_PIRQ_INTERRUPT
#define BOARD_CODEC_WAKE_GPIO		46	/* GPIO46 */
#define BOARD_CODEC_I2C_BUS		1	/* I2C0 */
#define BOARD_CODEC_I2C_ADDR		0x1a

#define BOARD_NFC_NAME			"nfc"
#define BOARD_NFC_IRQ			GPIO_INTERRUPT(9)
#define BOARD_NFC_IRQ_TYPE		BOARD_GPIO_INTERRUPT
#define BOARD_NFC_WAKE_GPIO		9	/* GPIO9 */
#define BOARD_NFC_I2C_BUS		1	/* I2C0 */
#define BOARD_NFC_I2C_ADDR		0x28

#define BOARD_ACCEL_NAME		"accel"
#define BOARD_ACCEL_IRQ			29	/* PIRQN */
#define BOARD_ACCEL_IRQ_TYPE		BOARD_PIRQ_INTERRUPT
#define BOARD_ACCEL_WAKE_GPIO		45	/* GPIO45 */
#define BOARD_ACCEL_I2C_BUS		2	/* I2C1 */
#define BOARD_ACCEL_I2C_ADDR		0x0e

#define BOARD_ACCEL_GYRO_NAME		"accel_gyro"
#define BOARD_ACCEL_GYRO_IRQ		31	/* PIRQP */
#define BOARD_ACCEL_GYRO_IRQ_TYPE	BOARD_PIRQ_INTERRUPT
#define BOARD_ACCEL_GYRO_WAKE_GPIO	47	/* GPIO47 */
#define BOARD_ACCEL_GYRO_I2C_BUS	2	/* I2C1 */
#define BOARD_ACCEL_GYRO_I2C_ADDR	0x6b

#endif
