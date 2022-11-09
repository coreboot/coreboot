/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <soc/addressmap.h>
#include <soc/sdram.h>
#include <symbols.h>

uintptr_t cbmem_top_chipset(void)
{
	return MIN((uintptr_t)_dram + sdram_size_mb() * MiB, FU540_MAXDRAM);
}
