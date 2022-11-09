/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/bsd/helpers.h>
#include <soc/display.h>
#include <soc/sdram.h>

uintptr_t cbmem_top_chipset(void)
{
	return (sdram_max_addressable_mb() - FB_SIZE_MB) * MiB;
}
