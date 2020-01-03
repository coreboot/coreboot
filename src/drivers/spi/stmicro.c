/*
 * This file is part of the coreboot project.
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
#include <commonlib/helpers.h>
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
#define STM_ID_M25PX80		0x7114
#define STM_ID_M25PX16		0x7115
#define STM_ID_M25PX32		0x7116
#define STM_ID_M25PX64		0x7117
#define STM_ID_M25PE80		0x8014
#define STM_ID_M25PE16		0x8015
#define STM_ID_M25PE32		0x8016
#define STM_ID_M25PE64		0x8017
#define STM_ID_N25Q016__3E	0xba15
#define STM_ID_N25Q032__3E	0xba16
#define STM_ID_N25Q064__3E	0xba17
#define STM_ID_N25Q128__3E	0xba18
#define STM_ID_N25Q256__3E	0xba19
#define STM_ID_N25Q016__1E	0xbb15
#define STM_ID_N25Q032__1E	0xbb16
#define STM_ID_N25Q064__1E	0xbb17
#define STM_ID_N25Q128__1E	0xbb18
#define STM_ID_N25Q256__1E	0xbb19

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
		.device_id = STM_ID_M25PX80,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 16,
		.name = "M25PX80",
	},
	{
		.device_id = STM_ID_M25PX16,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 32,
		.name = "M25PX16",
	},
	{
		.device_id = STM_ID_M25PX32,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "M25PX32",
	},
	{
		.device_id = STM_ID_M25PX64,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.name = "M25PX64",
	},
	{
		.device_id = STM_ID_M25PE80,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 16,
		.name = "M25PE80",
	},
	{
		.device_id = STM_ID_M25PE16,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 32,
		.name = "M25PE16",
	},
	{
		.device_id = STM_ID_M25PE32,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "M25PE32",
	},
	{
		.device_id = STM_ID_M25PE64,
		.op_erase = CMD_M25PXX_SE,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.name = "M25PE64",
	},
	{
		.device_id = STM_ID_N25Q016__3E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 512,
		.name = "N25Q016..3E",
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
		.device_id = STM_ID_N25Q064__3E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 2048,
		.name = "N25Q064..3E",
	},
	{
		.device_id = STM_ID_N25Q128__3E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 4096,
		.name = "N25Q128..3E",
	},
	{
		.device_id = STM_ID_N25Q256__3E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 8192,
		.name = "N25Q256..3E",
	},
	{
		.device_id = STM_ID_N25Q016__1E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 512,
		.name = "N25Q016..1E",
	},
	{
		.device_id = STM_ID_N25Q032__1E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 1024,
		.name = "N25Q032..1E",
	},
	{
		.device_id = STM_ID_N25Q064__1E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 2048,
		.name = "N25Q064..1E",
	},
	{
		.device_id = STM_ID_N25Q128__1E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 4096,
		.name = "N25Q128..1E",
	},
	{
		.device_id = STM_ID_N25Q256__1E,
		.op_erase = CMD_M25PXX_SSE,
		.page_size = 256,
		.pages_per_sector = 16,
		.nr_sectors = 8192,
		.name = "N25Q256..1E",
	},
};

static const struct spi_flash_ops spi_flash_ops = {
	.write = spi_flash_cmd_write_page_program,
	.erase = spi_flash_cmd_erase,
};

int stmicro_release_deep_sleep_identify(const struct spi_slave *spi, u8 *idcode)
{
	if (spi_flash_cmd(spi, CMD_M25PXX_RES, idcode, 4))
		return -1;

	/* Assuming ST parts identify with 0x1X to release from deep
	   power down and read electronic signature. */
	if ((idcode[3] & 0xf0) != 0x10)
		return -1;

	/* Fix up the idcode to mimic rdid jedec instruction. */
	idcode[0] = 0x20;
	idcode[1] = 0x20;
	idcode[2] = idcode[3] + 1;

	return 0;
}

int spi_flash_probe_stmicro(const struct spi_slave *spi, u8 *idcode,
			    struct spi_flash *flash)
{
	const struct stmicro_spi_flash_params *params;
	unsigned int i;

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
	flash->pp_cmd = CMD_M25PXX_PP;
	flash->wren_cmd = CMD_M25PXX_WREN;

	flash->ops = &spi_flash_ops;

	return 0;
}
