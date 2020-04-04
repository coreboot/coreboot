/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <soc/trustzone.h>

/* Setting TZPC[TrustZone Protection Controller]
 * We pretty much disable it all, as the kernel
 * expects it that way -- and that's not the default.
 */
void trustzone_init(void)
{
	struct exynos_tzpc *tzpc;
	unsigned int addr;

	for (addr = TZPC10_BASE; addr <= TZPC9_BASE; addr += TZPC_BASE_OFFSET) {
		tzpc = (struct exynos_tzpc *)addr;
		if (addr == TZPC0_BASE)
			write32(&tzpc->r0size, R0SIZE);
		write32(&tzpc->decprot0set, DECPROTXSET);
		write32(&tzpc->decprot1set, DECPROTXSET);
		write32(&tzpc->decprot2set, DECPROTXSET);
		write32(&tzpc->decprot3set, DECPROTXSET);
	}
}
