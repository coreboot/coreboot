/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/bsd/helpers.h>

uintptr_t cbmem_top_chipset(void)
{
	return CONFIG_DRAM_SIZE_MB * MiB;
}
