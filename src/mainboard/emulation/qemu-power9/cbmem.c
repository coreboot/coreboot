/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <ramdetect.h>

void *cbmem_top_chipset(void)
{
	return (void *)(probe_ramsize(0, CONFIG_DRAM_SIZE_MB) * MiB);
}
