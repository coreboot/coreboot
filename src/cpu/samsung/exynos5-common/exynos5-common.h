/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * Common configuration settings for EXYNOS5 based boards.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __EXYNOS5_CONFIG_H
#define __EXYNOS5_CONFIG_H

#include <cpu/samsung/exynos5250/cpu.h>		/* get chip and board defs */

/* TODO(dhendrix): some #defines are commented out here and moved to Kconfig */

//#define CONFIG_SYS_SDRAM_BASE		0x40000000
//#define CONFIG_SYS_TEXT_BASE		0x43e00000

/* Power Down Modes */
#define S5P_CHECK_SLEEP			0x00000BAD
#define S5P_CHECK_DIDLE			0xBAD00000
#define S5P_CHECK_LPA			0xABAD0000

#define CONFIG_SYS_HZ			1000

/* We spend about 100us getting from reset to SPL */
#define CONFIG_SPL_TIME_US	100000

#endif	/* __EXYNOS5_CONFIG_H */
