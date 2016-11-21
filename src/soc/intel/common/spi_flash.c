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

#include <arch/io.h>
#include <console/console.h>
#include "spi_flash.h"

/*
 * Protect range of SPI flash defined by [start, start+size-1] using Flash
 * Protected Range (FPR) register if available.
 */
int spi_flash_protect(u32 start, u32 size)
{
	struct fpr_info fpr_info;
	u32 end = start + size - 1;
	u32 reg;
	int fpr;
	uintptr_t fpr_base;

	if (spi_flash_get_fpr_info(&fpr_info) == -1) {
		printk(BIOS_ERR, "ERROR: FPR Info not found!\n");
		return -1;
	}

	fpr_base = fpr_info.base;

	/* Find first empty FPR */
	for (fpr = 0; fpr < fpr_info.max; fpr++) {
		reg = read32((void *)fpr_base);
		if (reg == 0)
			break;
		fpr_base += sizeof(uint32_t);
	}

	if (fpr >= fpr_info.max) {
		printk(BIOS_ERR, "ERROR: No SPI FPR free!\n");
		return -1;
	}

	/* Set protected range base and limit */
	reg = SPI_FPR(start, end) | SPI_FPR_WPE;

	/* Set the FPR register and verify it is protected */
	write32((void *)fpr_base, reg);
	reg = read32((void *)fpr_base);
	if (!(reg & SPI_FPR_WPE)) {
		printk(BIOS_ERR, "ERROR: Unable to set SPI FPR %d\n", fpr);
		return -1;
	}

	printk(BIOS_INFO, "%s: FPR %d is enabled for range 0x%08x-0x%08x\n",
	       __func__, fpr, start, end);
	return 0;
}
