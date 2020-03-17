/*
 * This file is part of the coreboot project.
 *
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

#include <device/mmio.h>
#include <soc/trustzone.h>

/* Setting TZPC[TrustZone Protection Controller] */
void trustzone_init(void)
{
	struct exynos_tzpc *tzpc;
	unsigned int addr;

	for (addr = TZPC0_BASE; addr <= TZPC9_BASE; addr += TZPC_BASE_OFFSET) {
		tzpc = (struct exynos_tzpc *)addr;

		if (addr == TZPC0_BASE)
			write32(&tzpc->r0size, R0SIZE);

		write32(&tzpc->decprot0set, DECPROTXSET);
		write32(&tzpc->decprot1set, DECPROTXSET);

		if (addr != TZPC9_BASE) {
			write32(&tzpc->decprot2set, DECPROTXSET);
			write32(&tzpc->decprot3set, DECPROTXSET);
		}
	}
}
