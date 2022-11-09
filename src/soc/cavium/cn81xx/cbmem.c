/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/addressmap.h>
#include <soc/sdram.h>
#include <symbols.h>

uintptr_t cbmem_top_chipset(void)
{
	/* Make sure not to overlap with reserved ATF scratchpad */
	return (uintptr_t)_dram + (sdram_size_mb() - 1) * MiB;
}
