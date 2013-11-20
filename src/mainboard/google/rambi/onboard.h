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

#include "irqroute.h"

#define BOARD_TRACKPAD_NAME             "trackpad"
#define BOARD_TRACKPAD_IRQ              GPIO_S0_DED_IRQ(TPAD_IRQ_OFFSET)
#define BOARD_TRACKPAD_WAKE_GPIO        1       /* GPSSUS1 */
#define BOARD_TRACKPAD_I2C_BUS          0
#define BOARD_TRACKPAD_I2C_ADDR         0x4b

#define BOARD_TOUCHSCREEN_NAME          "touchscreen"
#define BOARD_TOUCHSCREEN_IRQ           GPIO_S0_DED_IRQ(TOUCH_IRQ_OFFSET)
#define BOARD_TOUCHSCREEN_WAKE_GPIO     2       /* GPSSUS2 */
#define BOARD_TOUCHSCREEN_I2C_BUS       5
#define BOARD_TOUCHSCREEN_I2C_ADDR      0x4a    /* TODO(shawnn): Check this */

#define BOARD_I8042_IRQ                 GPIO_S0_DED_IRQ(I8042_IRQ_OFFSET)

#endif
