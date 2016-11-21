/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __INTEL_COMMON_SPI_FLASH_H__
#define __INTEL_COMMON_SPI_FLASH_H__

#define SPI_FPR_SHIFT		12
#define SPI_FPR_MASK		0x7fff
#define SPI_FPR_BASE_SHIFT	0
#define SPI_FPR_LIMIT_SHIFT	16
#define SPI_FPR_RPE		(1 << 15) /* Read Protect */
#define SPI_FPR_WPE		(1 << 31) /* Write Protect */
#define SPI_FPR(base, limit)	\
	(((((limit) >> SPI_FPR_SHIFT) & SPI_FPR_MASK) << SPI_FPR_LIMIT_SHIFT) |\
	 ((((base) >> SPI_FPR_SHIFT) & SPI_FPR_MASK) << SPI_FPR_BASE_SHIFT))

struct fpr_info {
	/* Offset of first FPR register */
	uintptr_t base;
	/* Maximum number of FPR registers */
	uint8_t max;
};

/*
 * SoC is expected to implement this function to provide address of first FPR
 * register and max count of FPR registers.
 *
 * On success return 0 else -1.
 */
int spi_flash_get_fpr_info(struct fpr_info *info);

/*
 * Protect range of SPI flash defined by [start, start+size-1] using Flash
 * Protected Range (FPR) register if available.
 */
int spi_flash_protect(u32 start, u32 size);

#endif /* __INTEL_COMMON_SPI_FLASH_H__ */
