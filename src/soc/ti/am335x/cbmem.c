/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/bsd/helpers.h>
#include <symbols.h>

uintptr_t cbmem_top_chipset(void)
{
	return (uintptr_t)_dram + CONFIG_DRAM_SIZE_MB * MiB;
}
