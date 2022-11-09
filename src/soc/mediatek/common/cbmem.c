/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <symbols.h>
#include <soc/emi.h>

#define MAX_DRAM_ADDRESS ((uintptr_t)4 * GiB)

uintptr_t cbmem_top_chipset(void)
{
	return MIN((uintptr_t)_dram + sdram_size(), MAX_DRAM_ADDRESS);
}
