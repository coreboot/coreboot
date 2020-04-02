/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stddef.h>
#include <cbmem.h>
#include <symbols.h>

void *cbmem_top_chipset(void)
{
	return _dram + (CONFIG_DRAM_SIZE_MB << 20);
}
