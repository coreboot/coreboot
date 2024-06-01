/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/memmap.h>
#include <amdblocks/smm.h>
#include <arch/vga.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <memrange.h>
#include <types.h>

struct memmap_early_dram {
	/* fixed size types, so the layout in CBMEM won't change for 32 vs. 64 bit stages */
	uint32_t base;
	uint32_t size;
};

void memmap_stash_early_dram_usage(void)
{
	struct memmap_early_dram *e;

	e = cbmem_add(CBMEM_ID_CB_EARLY_DRAM, sizeof(*e));

	if (!e)
		die("ERROR: Failed to stash early dram usage!\n");

	e->base = (uint32_t)(uintptr_t)_early_reserved_dram;
	e->size = REGION_SIZE(early_reserved_dram);
}

static const struct memmap_early_dram *memmap_get_early_dram_usage(void)
{
	struct memmap_early_dram *e = cbmem_find(CBMEM_ID_CB_EARLY_DRAM);

	if (!e)
		die("ERROR: Failed to read early dram usage!\n");

	return e;
}

/* report SoC memory map up to cbmem_top */
void read_lower_soc_memmap_resources(struct device *dev, unsigned long *idx)
{
	const uint32_t mem_usable = cbmem_top();
	const struct memmap_early_dram *e = memmap_get_early_dram_usage();
	const uintptr_t early_reserved_dram_start = e->base;
	const uintptr_t early_reserved_dram_end = e->base + e->size;

	/* 0x0 - 0x9ffff */
	ram_range(dev, (*idx)++, 0, VGA_MMIO_BASE);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_range(dev, (*idx)++, VGA_MMIO_BASE, VGA_MMIO_SIZE);

	/* 0xc0000 - 0xfffff: Option ROM */
	reserved_ram_from_to(dev, (*idx)++, VGA_MMIO_BASE + VGA_MMIO_SIZE, 1 * MiB);

	/* 1MiB - bottom of DRAM reserved for early coreboot usage */
	ram_from_to(dev, (*idx)++, 1 * MiB, early_reserved_dram_start);

	/* DRAM reserved for early coreboot usage */
	reserved_ram_from_to(dev, (*idx)++, early_reserved_dram_start, early_reserved_dram_end);

	/*
	 * top of DRAM consumed early - low top usable RAM
	 * cbmem_top() accounts for low UMA and TSEG if they are used.
	 */
	ram_from_to(dev, (*idx)++, early_reserved_dram_end, mem_usable);
}

void smm_region(uintptr_t *start, size_t *size)
{
	static int once;

	if (CONFIG(PLATFORM_USES_FSP2_0)) {
		fsp_get_smm_region(start, size);
	} else {
		*start = cbmem_top();
		*size = CONFIG_SMM_TSEG_SIZE;
	}

	if (!once) {
		clear_tvalid();
		once = 1;
	}
}
