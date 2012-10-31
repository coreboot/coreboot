/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/periph.h>

#include <cpu/samsung/exynos5250/uart.h>

enum periph_id exynos5_get_periph_id(unsigned base_addr)
{
	enum periph_id id = PERIPH_ID_NONE;

	switch (base_addr) {
	case EXYNOS5_UART0_BASE:
		id = PERIPH_ID_UART0;
		break;
	case EXYNOS5_UART1_BASE:
		id = PERIPH_ID_UART1;
		break;
	case EXYNOS5_UART2_BASE:
		id = PERIPH_ID_UART2;
		break;
	case EXYNOS5_UART3_BASE:
		id = PERIPH_ID_UART3;
		break;
	default:
		break;
	}

	return id;
}
