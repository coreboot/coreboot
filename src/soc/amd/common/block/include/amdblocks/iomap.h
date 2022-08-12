/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_IOMAP_H
#define AMD_BLOCK_IOMAP_H

/*
 * A maximum of 16 MBytes of the SPI flash can be mapped right below the 4 GB boundary. For
 * region reservation and cacheability configuration purposes, we can use this maximum value
 * and don't need to make this dependent on the flash size. This also makes sure that in case
 * of flash sizes above 16 MByte the MMIO region right below won't get configured wrongly.
 */
#define FLASH_BELOW_4GB_MAPPING_REGION_BASE	((0xffffffff - 16 * MiB) + 1)
#define FLASH_BELOW_4GB_MAPPING_REGION_SIZE	(16 * MiB)

#endif /* AMD_BLOCK_IOMAP_H */
