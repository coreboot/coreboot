/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <stdlib.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <string.h>

#include "spi_flash_internal.h"

/* M25Pxx-specific commands */
#define CMD_M25PXX_WREN		0x06	/* Write Enable */
#define CMD_M25PXX_WRDI		0x04	/* Write Disable */
#define CMD_M25PXX_RDSR		0x05	/* Read Status Register */
#define CMD_M25PXX_WRSR		0x01	/* Write Status Register */
#define CMD_M25PXX_READ		0x03	/* Read Data Bytes */
#define CMD_M25PXX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_M25PXX_PP		0x02	/* Page Program */
#define CMD_M25PXX_SSE		0x20	/* Subsector Erase */
#define CMD_M25PXX_SE		0xd8	/* Sector Erase */
#define CMD_M25PXX_BE		0xc7	/* Bulk Erase */
#define CMD_M25PXX_DP		0xb9	/* Deep Power-down */
#define CMD_M25PXX_RES		0xab	/* Release from DP, and Read Signature */

/*
 * Device ID = (memory_type << 8) + memory_capacity
 */
#define STM_ID_M25P10		0x2011
#define STM_ID_M25P20		0x2012
#define STM_ID_M25P40		0x2013
#define STM_ID_M25P80		0x2014
#define STM_ID_M25P16		0x2015
#define STM_ID_M25P32		0x2016
#define STM_ID_M25P64		0x2017
#define STM_ID_M25P128		0x2018
#define STM_ID_N25Q032__3E	0xba16
#define STM_ID_N25Q128A		0xba18
#define STM_ID_N25Q256		0xba19
#define STM_ID_N25Q064		0xbb17
#define STM_ID_N25Q128		0xbb18

struct stmicro_spi_flash_params {
	u16 device_id;
	u8 op_erase;
	u16 page_size;
	u16 pages_per_sector;
	u16 nr_sectors;
	const char *name;
};

static const struct stmicro_spi_flash_params stmicro_spi_flash_table[] = {
	{
		.device_id = STM_ID_M25P10,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 128,
		.nr_sectors = 4,
		.name = "M25P10",
	},
	{
		.device_id = STM_ID_M25P16,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 32,
		.name = "M25P16",
	},
	{
		.device_id = STM_ID_M25P20,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 4,
		.name = "M25P20",
	},
	{
		.device_id = STM_ID_M25P32,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "M25P32",
	},
	{
		.device_id = STM_ID_M25P40,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 8,
		.name = "M25P40",
	},
	{
		.device_id = STM_ID_M25P64,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.name = "M25P64",
	},
	{
		.device_id = STM_ID_M25P80,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 16,
		.name = "M25P80",
	},
	{
		.device_id = STM_ID_M25P128,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 1024,
		.nr_sectors = 64,
		.name = "M25P128",
	},
	{
		.device_id = STM_ID_N25Q032__3E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 1024,
		.name = "N25Q032..3E",
	},
	{
		.device_id = STM_ID_N25Q064,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 2048,
		.name = "N25Q064",
	},
	{
		.device_id = STM_ID_N25Q128,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 4096,
		.name = "N25Q128",
	},
	{
		.device_id = STM_ID_N25Q128A,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 4096,
		.name = "N25Q128A",
	},
	{
		.device_id = STM_ID_N25Q256,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 8192,
		.name = "N25Q256",
	},
};

static int stmicro_write(const struct spi_flash *flash,
			 u32 offset, size_t len, const void *buf)
{
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret = 0;
	u8 cmd[4];

	page_size = flash->page_size;

	for (actual = 0; actual < len; actual += chunk_len) {
		byte_addr = offset % page_size;
		chunk_len = min(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(&flash->spi, sizeof(cmd), chunk_len);

		cmd[0] = CMD_M25PXX_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;
#if IS_ENABLED(CONFIG_DEBUG_SPI_FLASH)
		printk(BIOS_SPEW, "PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x }"
		     " chunk_len = %zu\n",
		     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd(&flash->spi, CMD_M25PXX_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			goto out;
		}

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
					  buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: STMicro Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret)
			goto out;

		offset += chunk_len;
	}

#if IS_ENABLED(CONFIG_DEBUG_SPI_FLASH)
	printk(BIOS_SPEW, "SF: STMicro: Successfully programmed %zu bytes @"
			" 0x%lx\n", len, (unsigned long)(offset - len));
#endif
	ret = 0;

out:
	return ret;
}

static const struct spi_flash_ops spi_flash_ops = {
	.write = stmicro_write,
	.erase = spi_flash_cmd_erase,
	.read = spi_flash_cmd_read_fast,
};

int spi_flash_probe_stmicro(const struct spi_slave *spi, u8 *idcode,
			    struct spi_flash *flash)
{
	const struct stmicro_spi_flash_params *params;
	unsigned int i;

	if (idcode[0] == 0xff) {
		i = spi_flash_cmd(spi, CMD_M25PXX_RES, idcode, 4);
		if (i)
			return -1;
		if ((idcode[3] & 0xf0) == 0x10) {
			idcode[0] = 0x20;
			idcode[1] = 0x20;
			idcode[2] = idcode[3] + 1;
		} else
			return -1;
	}

	for (i = 0; i < ARRAY_SIZE(stmicro_spi_flash_table); i++) {
		params = &stmicro_spi_flash_table[i];
		if (params->device_id == (idcode[1] << 8 | idcode[2])) {
			break;
		}
	}

	if (i == ARRAY_SIZE(stmicro_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported STMicro ID %02x%02x\n",
		       idcode[1], idcode[2]);
		return -1;
	}

	memcpy(&flash->spi, spi, sizeof(*spi));
	flash->name = params->name;
	flash->page_size = params->page_size;
	flash->sector_size = params->page_size * params->pages_per_sector;
	flash->size = flash->sector_size * params->nr_sectors;
	flash->erase_cmd = params->op_erase;

	flash->ops = &spi_flash_ops;

	return 0;
}
