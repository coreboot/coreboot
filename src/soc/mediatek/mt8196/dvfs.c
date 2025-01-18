/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/cache.h>
#include <device/mmio.h>
#include <soc/dvfs.h>

void dvfs_init(void)
{
	uint32_t val;

	val = BIT(8) | BIT(0);

	write32p(CSRAM_BASE + 0x8, 0x55AA55AA);
	write32p(CSRAM_BASE + 0xC, val);

	dcache_clean_invalidate_by_mva((void *)CSRAM_BASE, CACHE_LINE);
}
