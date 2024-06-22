/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <symbols.h>
#include <ramdetect.h>
#include <commonlib/device_tree.h>
#include <mcall.h>

uintptr_t cbmem_top_chipset(void)
{
	uint64_t top;

	top = fdt_get_memory_top((void *)HLS()->fdt);
	if (top)
		return MIN(top, (uint64_t)4 * GiB - 1);

	size_t dram_mb_detected = probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB);
	return (uintptr_t)_dram + dram_mb_detected * MiB;
}
