/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/iomap.h>
#include <amdblocks/memmap.h>
#include <amdblocks/root_complex.h>
#include <device/device.h>
#include <stdint.h>
#include <vendorcode/amd/opensil/opensil.h>

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
	read_lower_soc_memmap_resources(dev, idx);

	/* Reserve fixed IOMMU MMIO region */
	mmio_range(dev, (*idx)++, IOMMU_RESERVED_MMIO_BASE, IOMMU_RESERVED_MMIO_SIZE);

	if (CONFIG(PLATFORM_USES_FSP2_0))
		read_fsp_resources(dev, idx);
	else
		add_opensil_memmap(dev, idx);
}
