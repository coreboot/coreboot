/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <soc/addressmap.h>
#include <soc/sdram.h>
#include <symbols.h>

void *cbmem_top_chipset(void)
{
	return (void *)MIN((uintptr_t)_dram + sdram_size_mb() * MiB,
			   MAX_DRAM_ADDRESS);
}
