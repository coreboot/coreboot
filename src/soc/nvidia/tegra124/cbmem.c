/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/display.h>
#include <soc/sdram.h>

void *cbmem_top_chipset(void)
{
	return (void *)((sdram_max_addressable_mb() - FB_SIZE_MB) << 20UL);
}
