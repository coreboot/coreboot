/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.  All rights reserved.
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
#include <delay.h>
#include <console/console.h>

#include <soc/iomap.h>
#include <soc/romstage.h>
#include <soc/spi.h>

#define SPI_CYCLE_DELAY 10				/* 10us */
#define SPI_CYCLE_TIMEOUT 400000 / SPI_CYCLE_DELAY	/* 400ms */

#define SPI8(x) *((volatile u8 *)(SPI_BASE_ADDRESS + x))
#define SPI16(x) *((volatile u16 *)(SPI_BASE_ADDRESS + x))
#define SPI32(x) *((volatile u32 *)(SPI_BASE_ADDRESS + x))

/* Minimal set of commands to read wpsr from SPI. Don't use this code outside
 * romstage -- it trashes the opmenu table.
 * Returns 0 on success, < 0 on failure. */
int early_spi_read_wpsr(u8 *sr)
{
	int timeout = SPI_CYCLE_TIMEOUT;

	/* No address associated with rdsr */
	SPI8(OPTYPE) = 0x0;
	/* Setup opcode[0] = read wpsr */
	SPI8(OPMENU0) = 0x5;

	/* Start transaction */
	SPI16(SSFC) = DATA_CYCLE | SPI_CYCLE_GO;

	/* Wait for error / complete status */
	while (timeout--) {
		u16 status = SPI16(SSFS);
		if (status & FLASH_CYCLE_ERROR) {
			printk(BIOS_ERR, "SPI rdsr failed\n");
			return -1;
		} else if (status & CYCLE_DONE_STATUS)
			break;

		udelay(SPI_CYCLE_DELAY);
	}

	*sr = SPI32(FDATA0) & 0xff;
	return 0;
}
