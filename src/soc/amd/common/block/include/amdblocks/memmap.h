/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_MEMMAP_H
#define AMD_BLOCK_MEMMAP_H

#include <device/device.h>
#include <stdint.h>
#include <symbols.h>

DECLARE_REGION(early_reserved_dram)

struct memmap_early_dram {
	/* fixed size types, so the layout in CBMEM won't change for 32 vs. 64 bit stages */
	uint32_t base;
	uint32_t size;
};

void memmap_stash_early_dram_usage(void);

/* report SoC memory map up to cbmem_top */
void read_lower_soc_memmap_resources(struct device *dev, unsigned long *idx);

void fsp_get_smm_region(uintptr_t *start, size_t *size);

#endif /* AMD_BLOCK_MEMMAP_H */
