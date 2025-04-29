/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/cache.h>
#include <device/mmio.h>
#include <soc/cpu_id.h>
#include <soc/dvfs.h>
#include <soc/symbols.h>

void dvfs_init(void)
{
	u32 id, val;

	id = get_cpu_segment_id();
	val = (id << 8) | BIT(0);

	write32(_dvfs2_reserved + 0x8, 0x55AA55AA);
	write32(_dvfs2_reserved + 0xC, val);

	dcache_clean_invalidate_by_mva(_dvfs2_reserved, 0x10);
}
