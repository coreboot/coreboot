/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/iomap.h>
#include <amdblocks/memmap.h>
#include <amdblocks/root_complex.h>
#include <arch/vga.h>
#include <cbmem.h>
#include <device/device.h>
#include <stdint.h>
#include <vendorcode/amd/opensil/stub/opensil.h>

/*
 *                     +--------------------------------+
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *                     |                                |
 *   reserved_dram_end +--------------------------------+
 *                     |                                |
 *                     |       verstage (if reqd)       |
 *                     |          (VERSTAGE_SIZE)       |
 *                     +--------------------------------+ VERSTAGE_ADDR
 *                     |                                |
 *                     |            FSP-M               |
 *                     |         (FSP_M_SIZE)           |
 *                     +--------------------------------+ FSP_M_ADDR
 *                     |           romstage             |
 *                     |        (ROMSTAGE_SIZE)         |
 *                     +--------------------------------+ ROMSTAGE_ADDR = BOOTBLOCK_END
 *                     |                                | X86_RESET_VECTOR = BOOTBLOCK_END  - 0x10
 *                     |           bootblock            |
 *                     |     (C_ENV_BOOTBLOCK_SIZE)     |
 *                     +--------------------------------+ BOOTBLOCK_ADDR = BOOTBLOCK_END - C_ENV_BOOTBLOCK_SIZE
 *                     |          Unused hole           |
 *                     |            (30KiB)             |
 *                     +--------------------------------+
 *                     |     FMAP cache (FMAP_SIZE)     |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + PSP_SHAREDMEM_SIZE + PRERAM_CBMEM_CONSOLE_SIZE + 0x200
 *                     |  Early Timestamp region (512B) |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + PSP_SHAREDMEM_SIZE + PRERAM_CBMEM_CONSOLE_SIZE
 *                     |      Preram CBMEM console      |
 *                     |   (PRERAM_CBMEM_CONSOLE_SIZE)  |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE + PSP_SHAREDMEM_SIZE
 *                     |   PSP shared (vboot workbuf)   |
 *                     |      (PSP_SHAREDMEM_SIZE)      |
 *                     +--------------------------------+ PSP_SHAREDMEM_BASE
 *                     |          APOB (120KiB)         |
 *                     +--------------------------------+ PSP_APOB_DRAM_ADDRESS
 *                     |        Early BSP stack         |
 *                     |   (EARLYRAM_BSP_STACK_SIZE)    |
 * reserved_dram_start +--------------------------------+ EARLY_RESERVED_DRAM_BASE
 *                     |              DRAM              |
 *                     +--------------------------------+ 0x100000
 *                     |           Option ROM           |
 *                     +--------------------------------+ 0xc0000
 *                     |           Legacy VGA           |
 *                     +--------------------------------+ 0xa0000
 *                     |              DRAM              |
 *                     +--------------------------------+ 0x0
 */
void read_soc_memmap_resources(struct device *dev, unsigned long *idx)
{
	uint32_t mem_usable = (uintptr_t)cbmem_top();

	uintptr_t early_reserved_dram_start, early_reserved_dram_end;
	const struct memmap_early_dram *e = memmap_get_early_dram_usage();

	early_reserved_dram_start = e->base;
	early_reserved_dram_end = e->base + e->size;

	/* 0x0 - 0x9ffff */
	ram_range(dev, (*idx)++, 0, 0xa0000);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_range(dev, (*idx)++, VGA_MMIO_BASE, VGA_MMIO_SIZE);

	/* 0xc0000 - 0xfffff: Option ROM */
	reserved_ram_from_to(dev, (*idx)++, 0xc0000, 1 * MiB);

	/* 1MiB - bottom of DRAM reserved for early coreboot usage */
	ram_from_to(dev, (*idx)++, 1 * MiB, early_reserved_dram_start);

	/* DRAM reserved for early coreboot usage */
	reserved_ram_from_to(dev, (*idx)++, early_reserved_dram_start, early_reserved_dram_end);

	/*
	 * top of DRAM consumed early - low top usable RAM
	 * cbmem_top() accounts for low UMA and TSEG if they are used.
	 */
	ram_from_to(dev, (*idx)++, early_reserved_dram_end, mem_usable);

	/* Reserve fixed IOMMU MMIO region */
	mmio_range(dev, (*idx)++, IOMMU_RESERVED_MMIO_BASE, IOMMU_RESERVED_MMIO_SIZE);

	if (CONFIG(PLATFORM_USES_FSP2_0))
		read_fsp_resources(dev, idx);
	else
		add_opensil_memmap(dev, idx);
}
