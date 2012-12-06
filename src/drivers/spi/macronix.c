/*
 * Copyright 2009(C) Marvell International Ltd. and its affiliates
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Based on drivers/mtd/spi/stmicro.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdlib.h>
#include <spi_flash.h>
#include "spi_flash_internal.h"

/* MX25xx-specific commands */
#define CMD_MX25XX_WREN		0x06	/* Write Enable */
#define CMD_MX25XX_WRDI		0x04	/* Write Disable */
#define CMD_MX25XX_RDSR		0x05	/* Read Status Register */
#define CMD_MX25XX_WRSR		0x01	/* Write Status Register */
#define CMD_MX25XX_READ		0x03	/* Read Data Bytes */
#define CMD_MX25XX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_MX25XX_PP		0x02	/* Page Program */
#define CMD_MX25XX_SE		0x20	/* Sector Erase */
#define CMD_MX25XX_BE		0xD8	/* Block Erase */
#define CMD_MX25XX_CE		0xc7	/* Chip Erase */
#define CMD_MX25XX_DP		0xb9	/* Deep Power-down */
#define CMD_MX25XX_RES		0xab	/* Release from DP, and Read Signature */

#define MACRONIX_SR_WIP		(1 << 0)	/* Write-in-Progress */

struct macronix_spi_flash_params {
	u16 idcode;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_blocks;
	const char *name;
};

struct macronix_spi_flash {
	struct spi_flash flash;
	const struct macronix_spi_flash_params *params;
};

static inline struct macronix_spi_flash *to_macronix_spi_flash(struct spi_flash
							       *flash)
{
	return container_of(flash, struct macronix_spi_flash, flash);
}

static const struct macronix_spi_flash_params macronix_spi_flash_table[] = {
	{
		.idcode = 0x2015,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 32,
		.name = "MX25L1605D",
	},
	{
		.idcode = 0x2016,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 64,
		.name = "MX25L3205D",
	},
	{
		.idcode = 0x5e16,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 64,
		.name = "MX25L3235D", /* MX25L3225D/MX25L3235D/MX25L3237D */
	},
	{
		.idcode = 0x2017,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 128,
		.name = "MX25L6405D",
	},
	{
		.idcode = 0x2018,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25L12805D",
	},
	{
		.idcode = 0x2618,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25L12855E",
	},
};

static int macronix_write(struct spi_flash *flash,
			  u32 offset, size_t len, const void *buf)
{
	struct macronix_spi_flash *mcx = to_macronix_spi_flash(flash);
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret;
	u8 cmd[4];

	page_size = min(mcx->params->page_size, CONTROLLER_PAGE_LIMIT);
	byte_addr = offset % page_size;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		printk(BIOS_WARNING, "SF: Unable to claim SPI bus\n");
		return ret;
	}

	ret = 0;
	for (actual = 0; actual < len; actual += chunk_len) {
		chunk_len = min(len - actual, page_size - byte_addr);

		cmd[0] = CMD_MX25XX_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;

#if CONFIG_DEBUG_SPI_FLASH
		printk(BIOS_SPEW, "PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x }"
		     " chunk_len = %zu\n",
		     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd(flash->spi, CMD_MX25XX_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, 4,
					  buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Macronix Page Program failed\n");
			break;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret)
			break;

		offset += chunk_len;
		byte_addr = 0;
	}

	printk(BIOS_INFO, "SF: Macronix: Successfully programmed %zu bytes @"
	      " 0x%lx\n", len, (unsigned long)(offset - len));

	spi_release_bus(flash->spi);
	return ret;
}

static int macronix_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	return spi_flash_cmd_erase(flash, CMD_MX25XX_SE, offset, len);
}

struct spi_flash *spi_flash_probe_macronix(struct spi_slave *spi, u8 *idcode)
{
	const struct macronix_spi_flash_params *params;
	struct macronix_spi_flash *mcx;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;

	for (i = 0; i < ARRAY_SIZE(macronix_spi_flash_table); i++) {
		params = &macronix_spi_flash_table[i];
		if (params->idcode == id)
			break;
	}

	if (i == ARRAY_SIZE(macronix_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported Macronix ID %04x\n", id);
		return NULL;
	}

	mcx = malloc(sizeof(*mcx));
	if (!mcx) {
		printk(BIOS_WARNING, "SF: Failed to allocate memory\n");
		return NULL;
	}

	mcx->params = params;
	mcx->flash.spi = spi;
	mcx->flash.name = params->name;

	mcx->flash.write = macronix_write;
	mcx->flash.erase = macronix_erase;
#if CONFIG_SPI_FLASH_NO_FAST_READ
	mcx->flash.read = spi_flash_cmd_read_slow;
#else
	mcx->flash.read = spi_flash_cmd_read_fast;
#endif
	mcx->flash.sector_size = params->page_size * params->pages_per_sector;
	mcx->flash.size = mcx->flash.sector_size * params->sectors_per_block *
		params->nr_blocks;

	return &mcx->flash;
}
