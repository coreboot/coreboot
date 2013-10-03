/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA_PINMUX_H__
#define __SOC_NVIDIA_TEGRA_PINMUX_H__

#include <stdint.h>

void pinmux_set_config(int pin_index, uint32_t config);
uint32_t pinmux_get_config(int pin_index);

enum {
	PINMUX_FUNC_MASK = 3 << 0,

	PINMUX_PULL_MASK = 3 << 2,
	PINMUX_PULL_NONE = 0 << 2,
	PINMUX_PULL_DOWN = 1 << 2,
	PINMUX_PULL_UP = 2 << 2,

	PINMUX_TRISTATE = 1 << 4,
	PINMUX_INPUT_ENABLE = 1 << 5,
	PINMUX_OPEN_DRAIN = 1 << 6,
	PINMUX_LOCK = 1 << 7,
	PINMUX_IO_RESET = 1 << 8,
	PINMUX_RCV_SEL = 1 << 9
};

#endif	/* __SOC_NVIDIA_TEGRA_PINMUX_H__ */
