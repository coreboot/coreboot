/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __COREBOOT_SRC_MAINBOARD_GOOGLE_ROTOR_BOARD_H
#define __COREBOOT_SRC_MAINBOARD_GOOGLE_ROTOR_BOARD_H

#include <stdint.h>

#include <soc/pinmux.h>

#define PINMUX_APB_UART0_TXD	PINMUX(165, 1,  1, 0, 0, PULLUP)
#define PINMUX_APB_UART0_RXD	PINMUX(166, 1,  1, 0, 0, PULLUP)
#define PINMUX_APB_UART1_TXD	PINMUX(176, 1,  1, 0, 0, PULLUP)
#define PINMUX_APB_UART1_RXD	PINMUX(177, 1,  1, 0, 0, PULLUP)

#endif  /* __COREBOOT_SRC_MAINBOARD_GOOGLE_ROTOR_BOARD_H */
