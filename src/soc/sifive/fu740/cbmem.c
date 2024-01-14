/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <soc/addressmap.h>
#include <soc/sdram.h>
#include <symbols.h>

uintptr_t cbmem_top_chipset(void)
{
#define FU740_MAXDRAM 0x800000000ULL // 32 GiB
	return MIN((uintptr_t)FU740_DRAM + sdram_size(), FU740_MAXDRAM);
}
