/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <delay.h>
#include <soc/spi.h>
#include <soc/rcba.h>
#include <soc/romstage.h>

#define SPI_DELAY 10     /* 10us */
#define SPI_RETRY 200000 /* 2s */

static int early_spi_read_block(u32 offset, u8 size, u8 *buffer)
{
	u32 *ptr32 = (u32*)buffer;
	u32 i;

	/* Clear status bits */
	SPIBAR16(SPIBAR_HSFS) |= SPIBAR_HSFS_AEL | SPIBAR_HSFS_FCERR |
		SPIBAR_HSFS_FDONE;

	if (SPIBAR16(SPIBAR_HSFS) & SPIBAR_HSFS_SCIP) {
		printk(BIOS_ERR, "SPI ERROR: transaction in progress\n");
		return -1;
	}

	/* Set flash address */
	SPIBAR32(SPIBAR_FADDR) = offset;

	/* Setup read transaction */
	SPIBAR16(SPIBAR_HSFC) = SPIBAR_HSFC_BYTE_COUNT(size) |
		SPIBAR_HSFC_CYCLE_READ;

	/* Start transaction */
	SPIBAR16(SPIBAR_HSFC) |= SPIBAR_HSFC_GO;

	/* Wait for completion */
	for (i = 0; i < SPI_RETRY; i++) {
		if (SPIBAR16(SPIBAR_HSFS) & SPIBAR_HSFS_SCIP) {
			/* Cycle in progress, wait 1ms */
			udelay(SPI_DELAY);
			continue;
		}

		if (SPIBAR16(SPIBAR_HSFS) & SPIBAR_HSFS_AEL) {
			printk(BIOS_ERR, "SPI ERROR: Access Error\n");
			return -1;

		}

		if (SPIBAR16(SPIBAR_HSFS) & SPIBAR_HSFS_FCERR) {
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
			*ptr32++ = SPIBAR32(SPIBAR_FDATA(i/sizeof(u32)));
		} else {
			/* reading < dword */
			u8 j, *ptr8 = (u8*)ptr32;
			u32 temp = SPIBAR32(SPIBAR_FDATA(i/sizeof(u32)));
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

/*
 * Minimal set of commands to read WPSR from SPI.
 * Don't use this code outside romstage -- it trashes the opmenu table.
 * Returns 0 on success, < 0 on failure.
 */
int early_spi_read_wpsr(u8 *sr)
{
	int retry;

	/* No address associated with rdsr */
	SPIBAR8(SPIBAR_OPTYPE) = 0x0;
	/* Setup opcode[0] = read wpsr */
	SPIBAR8(SPIBAR_OPMENU_LOWER) = 0x5;

	/* Start transaction */
	SPIBAR16(SPIBAR_SSFC) = SPIBAR_SSFC_DATA | SPIBAR_SSFC_GO;

	/* Wait for error / complete status */
	for (retry = SPI_RETRY; retry; retry--) {
		u16 status = SPIBAR16(SPIBAR_SSFS);
		if (status & SPIBAR_SSFS_ERROR) {
			printk(BIOS_ERR, "SPI rdsr failed\n");
			return -1;
		} else if (status & SPIBAR_SSFS_DONE) {
			break;
		}

		udelay(SPI_DELAY);
	}

	*sr = SPIBAR32(SPIBAR_FDATA(0)) & 0xff;
	return 0;
}
