/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_FAST_SPI_DMA_H
#define SOC_INTEL_COMMON_BLOCK_FAST_SPI_DMA_H

#include <commonlib/region.h>

#if CONFIG(FAST_SPI_DMA) && ENV_RAMSTAGE
/*
 * Check if Fast SPI DMA is supported by the current device.
 *
 * This function determines whether Fast SPI DMA operations are supported by the current
 * hardware configuration. It performs a one-time initialization to check the PCI vendor
 * ID and the Fast SPI DMA control register. If the vendor ID is invalid or the control
 * register does not indicate support for the BIOS region, the function returns false.
 * Otherwise, it returns true.
 *
 * @return True if Fast SPI DMA is supported; false otherwise.
 */
bool fast_spi_dma_is_supported(void);
#else
static inline bool fast_spi_dma_is_supported(void)
{
	return false;
}
#endif

/*
 * Install custom DMA operations for SPI flash memory access.
 *
 * This function sets up a specialized set of operations to access SPI flash memory
 * regions using DMA transfers. The new operation set is derived from the original
 * one. Only the original readat() callback is replaced by a DMA-specific version. The
 * original operations are kept available for fallback use. The function is meant to run
 * only once, ensuring the proper installation of the custom operations.
 *
 * @param mmap_xlate_rdev Pointer to the xlate_region_device structure, which contains
 *                        information about the memory-mapped regions and operations.
 */
void spi_flash_dma_install_ops(struct xlate_region_device *mmap_xlate_rdev);

#endif /* SOC_INTEL_COMMON_BLOCK_FAST_SPI_DMA_H */
