/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <ramdetect.h>
#include <symbols.h>

uintptr_t cbmem_top_chipset(void)
{
	return (uintptr_t)_dram + (probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB) * MiB);
}
