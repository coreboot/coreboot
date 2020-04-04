/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <stddef.h>
#include <symbols.h>
#include <soc/emi.h>

#define MAX_DRAM_ADDRESS ((uintptr_t)4 * GiB)

void *cbmem_top_chipset(void)
{
	return (void *)MIN((uintptr_t)_dram + sdram_size(), MAX_DRAM_ADDRESS);
}
