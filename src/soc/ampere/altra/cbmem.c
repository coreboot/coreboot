/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <console/console.h>
#include <ramdetect.h>
#include <symbols.h>
#include <arch/stages.h>

uintptr_t cbmem_top_chipset(void)
{
	return (uintptr_t)_dram + (uint64_t)CONFIG_DRAM_SIZE_MB * MiB;
}
