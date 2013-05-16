/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

#ifndef CPU_SAMSUNG_EXYNOS5420_PINMUX_H
#define CPU_SAMSUNG_EXYNOS5420_PINMUX_H

#include "periph.h"

enum {
	PINMUX_FLAG_NONE	= 0x00000000,

	/* Flags for eMMC */
	PINMUX_FLAG_8BIT_MODE	= 1 << 0,	/* SDMMC 8-bit mode */

	/*
	 * Flags for SPI.
	 */
	PINMUX_FLAG_SLAVE_MODE	= 1 << 0,	/* Slave mode */

	/* Flags for SROM controller */
	PINMUX_FLAG_BANK	= 3 << 0,	/* bank number (0-3) */
	PINMUX_FLAG_16BIT	= 1 << 2,	/* 16-bit width */
};

/**
 * Configures the pinmux for a particular peripheral.
 *
 * Each gpio can be configured in many different ways (4 bits on exynos)
 * such as "input", "output", "special function", "external interrupt"
 * etc. This function will configure the peripheral pinmux along with
 * pull-up/down and drive strength.
 *
 * @param peripheral	peripheral to be configured
 * @param flags		configure flags
 * @return 0 if ok, -1 on error (e.g. unsupported peripheral)
 */
int exynos_pinmux_config(enum periph_id peripheral, int flags);

#endif
