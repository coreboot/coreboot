/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <symbols.h>
#include <ramdetect.h>
#include <console/console.h>

uintptr_t cbmem_top_chipset(void)
{
	//TODO get memory range from QEMUs FDT
	size_t dram_mb_detected = probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB);
	return (uintptr_t)_dram + dram_mb_detected * MiB;
}
