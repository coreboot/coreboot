/*
 * Lowlevel setup for SMDK5250 board based on S5PC520
 *
 * Copyright (C) 2012 Samsung Electronics
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

#include <cpu/samsung/exynos5250/cpu.h>
#include <asm/arch/dmc.h>
#include <asm/arch/tzpc.h>
#include"setup.h"

/* Setting TZPC[TrustZone Protection Controller] */
void tzpc_init(void)
{
	struct exynos5_tzpc *tzpc;
	unsigned int addr;

	for (addr = TZPC0_BASE; addr <= TZPC9_BASE; addr += TZPC_BASE_OFFSET) {
		tzpc = (struct exynos5_tzpc *)addr;

		if (addr == TZPC0_BASE)
			writel(R0SIZE, &tzpc->r0size);

		writel(DECPROTXSET, &tzpc->decprot0set);
		writel(DECPROTXSET, &tzpc->decprot1set);

		if (addr == TZPC9_BASE) {

		/* TODO: Add comment here describing the numerical values
		 * used below.
		 */
			writel(0xf0, &tzpc->decprot2set);
			writel(0x50, &tzpc->decprot3set);
		} else {
			writel(DECPROTXSET, &tzpc->decprot2set);
			writel(DECPROTXSET, &tzpc->decprot3set);
		}
	}
}
