/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/* EN25*-specific commands */
#define CMD_EN25_WREN		0x06	/* Write Enable */
#define CMD_EN25_WRDI		0x04	/* Write Disable */
#define CMD_EN25_RDSR		0x05	/* Read Status Register */
#define CMD_EN25_WRSR		0x01	/* Write Status Register */
#define CMD_EN25_READ		0x03	/* Read Data Bytes */
#define CMD_EN25_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_EN25_PP		0x02	/* Page Program */
#define CMD_EN25_SE		0x20	/* Sector Erase */
#define CMD_EN25_BE		0xd8	/* Block Erase */
#define CMD_EN25_DP		0xb9	/* Deep Power-down */
#define CMD_EN25_RES		0xab	/* Release from DP, and Read Signature */

#define EON_ID_EN25B80		0x2014
#define EON_ID_EN25B16		0x2015
#define EON_ID_EN25B32		0x2016
#define EON_ID_EN25B64		0x2017
#define EON_ID_EN25F80		0x3114
#define EON_ID_EN25F16		0x3115
#define EON_ID_EN25F32		0x3116
#define EON_ID_EN25F64		0x3117
#define EON_ID_EN25Q80		0x3014
#define EON_ID_EN25Q16		0x3015	/* Same as EN25D16 */
#define EON_ID_EN25Q32		0x3016	/* Same as EN25Q32A and EN25Q32B */
#define EON_ID_EN25Q64		0x3017
#define EON_ID_EN25Q128		0x3018
#define EON_ID_EN25QH16		0x7015
#define EON_ID_EN25QH32		0x7016
#define EON_ID_EN25QH64		0x7017
#define EON_ID_EN25QH128	0x7018
#define EON_ID_EN25S80		0x3814
#define EON_ID_EN25S16		0x3815
#define EON_ID_EN25S32		0x3816
#define EON_ID_EN25S64		0x3817

struct eon_spi_flash_params {
	u16 id;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_sectors;
	const char *name;
};

static const struct eon_spi_flash_params eon_spi_flash_table[] = {
	{
		.id = EON_ID_EN25B80,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 256,
		.name = "EN25B80",
	},
	{
		.id = EON_ID_EN25B16,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 512,
		.name = "EN25B16",
	},
	{
		.id = EON_ID_EN25B32,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 1024,
		.name = "EN25B32",
	},
	{
		.id = EON_ID_EN25B64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25B64",
	},
	{
		.id = EON_ID_EN25F80,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 256,
		.name = "EN25F80",
	},
	{
		.id = EON_ID_EN25F16,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 512,
		.name = "EN25F16",
	},
	{
		.id = EON_ID_EN25F32,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 1024,
		.name = "EN25F32",
	},
	{
		.id = EON_ID_EN25F64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25F64",
	},
	{
		.id = EON_ID_EN25Q80,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 256,
		.name = "EN25Q80(A)",
	},
	{
		.id = EON_ID_EN25Q16,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 512,
		.name = "EN25Q16(D16)",
	},
	{
		.id = EON_ID_EN25Q32,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 1024,
		.name = "EN25Q32(A/B)",
	},
	{
		.id = EON_ID_EN25Q64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25Q64",
	},
	{
		.id = EON_ID_EN25Q128,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 4096,
		.name = "EN25Q128",
	},
	{
		.id = EON_ID_EN25QH16,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 512,
		.name = "EN25QH16",
	},
	{
		.id = EON_ID_EN25QH32,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 1024,
		.name = "EN25QH32",
	},
	{
		.id = EON_ID_EN25QH64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25QH64",
	},
	{
		.id = EON_ID_EN25QH128,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 4096,
		.name = "EN25QH128",
	},
	{
		.id = EON_ID_EN25S80,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 256,
		.name = "EN25S80",
	},
	{
		.id = EON_ID_EN25S16,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 512,
		.name = "EN25S16",
	},
	{
		.id = EON_ID_EN25S32,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 1024,
		.name = "EN25S32",
	},
	{
		.id = EON_ID_EN25S64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25S64",
	},
};

static int eon_write(const struct spi_flash *flash,
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
		chunk_len = MIN(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(&flash->spi, sizeof(cmd), chunk_len);

		ret = spi_flash_cmd(&flash->spi, CMD_EN25_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			goto out;
		}

		cmd[0] = CMD_EN25_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;

#if CONFIG(DEBUG_SPI_FLASH)
		printk(BIOS_SPEW,
		    "PP: %p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %zu\n",
		     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
					  buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: EON Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash,
				SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret) {
			printk(BIOS_WARNING, "SF: EON Page Program timeout\n");
			goto out;
		}

		offset += chunk_len;
	}

#if CONFIG(DEBUG_SPI_FLASH)
	printk(BIOS_SPEW, "SF: EON: Successfully programmed %zu bytes @ %#x\n",
	       len, (unsigned int)(offset - len));
#endif

out:
	return ret;
}

static const struct spi_flash_ops spi_flash_ops = {
	.write = eon_write,
	.erase = spi_flash_cmd_erase,
	.status = spi_flash_cmd_status,
};

int spi_flash_probe_eon(const struct spi_slave *spi, u8 *idcode,
			struct spi_flash *flash)
{
	const struct eon_spi_flash_params *params;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(eon_spi_flash_table); ++i) {
		params = &eon_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(eon_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported EON ID %#02x%02x\n",
		       idcode[1], idcode[2]);
		return -1;
	}

	memcpy(&flash->spi, spi, sizeof(*spi));

	flash->name = params->name;
	flash->page_size = params->page_size;
	flash->sector_size = params->page_size * params->pages_per_sector;
	flash->size = flash->sector_size * params->nr_sectors;
	flash->erase_cmd = CMD_EN25_SE;
	flash->status_cmd = CMD_EN25_RDSR;

	flash->ops = &spi_flash_ops;

	return 0;
}
