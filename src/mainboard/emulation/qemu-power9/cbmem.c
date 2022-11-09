/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <ramdetect.h>

uintptr_t cbmem_top_chipset(void)
{
	return probe_ramsize(0, CONFIG_DRAM_SIZE_MB) * MiB;
}
