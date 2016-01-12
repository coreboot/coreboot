/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <delay.h>
#include "pch.h"

#define SPI_DELAY 10     /* 10us */
#define SPI_RETRY 200000 /* 2s */

static int early_spi_read_block(u32 offset, u8 size, u8 *buffer)
{
	u32 *ptr32 = (u32*)buffer;
	u32 i;

	/* Clear status bits */
	RCBA16(SPIBAR_HSFS) |= SPIBAR_HSFS_AEL | SPIBAR_HSFS_FCERR |
		SPIBAR_HSFS_FDONE;

	if (RCBA16(SPIBAR_HSFS) & SPIBAR_HSFS_SCIP) {
		printk(BIOS_ERR, "SPI ERROR: transaction in progress\n");
		return -1;
	}

	/* Set flash address */
	RCBA32(SPIBAR_FADDR) = offset;

	/* Setup read transaction */
	RCBA16(SPIBAR_HSFC) = SPIBAR_HSFC_BYTE_COUNT(size) |
		SPIBAR_HSFC_CYCLE_READ;

	/* Start transactinon */
	RCBA16(SPIBAR_HSFC) |= SPIBAR_HSFC_GO;

	/* Wait for completion */
	for (i = 0; i < SPI_RETRY; i++) {
		if (RCBA16(SPIBAR_HSFS) & SPIBAR_HSFS_SCIP) {
			/* Cycle in progress, wait 1ms */
			udelay(SPI_DELAY);
			continue;
		}

		if (RCBA16(SPIBAR_HSFS) & SPIBAR_HSFS_AEL) {
			printk(BIOS_ERR, "SPI ERROR: Access Error\n");
			return -1;

		}

		if (RCBA16(SPIBAR_HSFS) & SPIBAR_HSFS_FCERR) {
			printk(BIOS_ERR, "SPI ERROR: Flash Cycle Error\n");
			return -1;
		}
		break;
	}

	if (i >= SPI_RETRY) {
		printk(BIOS_ERR, "SPI ERROR: Timeout\n");
		return -1;
	}

	/* Read the data */
	for (i = 0; i < size; i+=sizeof(u32)) {
		if (size-i >= 4) {
			/* reading >= dword */
			*ptr32++ = RCBA32(SPIBAR_FDATA(i/sizeof(u32)));
		} else {
			/* reading < dword */
			u8 j, *ptr8 = (u8*)ptr32;
			u32 temp = RCBA32(SPIBAR_FDATA(i/sizeof(u32)));
			for (j = 0; j < (size-i); j++) {
				*ptr8++ = temp & 0xff;
				temp >>= 8;
			}
		}
	}

	return size;
}

int early_spi_read(u32 offset, u32 size, u8 *buffer)
{
	u32 current = 0;

	while (size > 0) {
		u8 count = (size < 64) ? size : 64;
		if (early_spi_read_block(offset + current, count,
					 buffer + current) < 0)
			return -1;
		size -= count;
		current += count;
	}

	return 0;
}
