/*
 * (C) Copyright 2012 Samsung Electronics
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
 *
 */

#ifndef __ASM_ARM_ARCH_EXYNOS5_MSHC_H__
#define __ASM_ARM_ARCH_EXYNOS5_MSHC_H__

#include <asm/arch-exynos/mshc.h>

#define MAX_MSHCI_CLOCK	52000000 /* Max limit for mshc clock is 52MHz */
#define MIN_MSHCI_CLOCK	400000 /* Lower limit for mshc clock is 400KHz */
#define COMMAND_TIMEOUT 10000
#define TIMEOUT_MS	100

#endif  /* __ASM_ARM_ARCH_EXYNOS5_MSHC_H__ */
