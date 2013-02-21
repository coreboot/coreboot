/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#ifndef LINK_ONBOARD_H
#define LINK_ONBOARD_H

#include <arch/smp/mpspec.h>

#define LINK_LIGHTSENSOR_NAME		"lightsensor"
#define LINK_LIGHTSENSOR_I2C_ADDR	0x44
#define LINK_LIGHTSENSOR_IRQ		0

#define LINK_TRACKPAD_NAME		"trackpad"
#define LINK_TRACKPAD_I2C_ADDR		0x4b
#define LINK_TRACKPAD_IRQ		20

#define LINK_TOUCHSCREEN_NAME		"touchscreen"
#define LINK_TOUCHSCREEN_I2C_ADDR	0x4a
#define LINK_TOUCHSCREEN_IRQ		22

#endif
