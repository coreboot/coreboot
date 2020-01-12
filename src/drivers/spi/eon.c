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

static const struct spi_flash_part_id flash_table[] = {
	{
		.id = EON_ID_EN25B80,
		.name = "EN25B80",
		.nr_sectors_shift = 8,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25B16,
		.name = "EN25B16",
		.nr_sectors_shift = 9,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25B32,
		.name = "EN25B32",
		.nr_sectors_shift = 10,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25B64,
		.name = "EN25B64",
		.nr_sectors_shift = 11,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25F80,
		.name = "EN25F80",
		.nr_sectors_shift = 8,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25F16,
		.name = "EN25F16",
		.nr_sectors_shift = 9,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25F32,
		.name = "EN25F32",
		.nr_sectors_shift = 10,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25F64,
		.name = "EN25F64",
		.nr_sectors_shift = 11,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25Q80,
		.name = "EN25Q80(A)",
		.nr_sectors_shift = 8,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25Q16,
		.name = "EN25Q16(D16)",
		.nr_sectors_shift = 9,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25Q32,
		.name = "EN25Q32(A/B)",
		.nr_sectors_shift = 10,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25Q64,
		.name = "EN25Q64",
		.nr_sectors_shift = 11,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25Q128,
		.name = "EN25Q128",
		.nr_sectors_shift = 12,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25QH16,
		.name = "EN25QH16",
		.nr_sectors_shift = 9,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25QH32,
		.name = "EN25QH32",
		.nr_sectors_shift = 10,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25QH64,
		.name = "EN25QH64",
		.nr_sectors_shift = 11,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25QH128,
		.name = "EN25QH128",
		.nr_sectors_shift = 12,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25S80,
		.name = "EN25S80",
		.nr_sectors_shift = 8,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25S16,
		.name = "EN25S16",
		.nr_sectors_shift = 9,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25S32,
		.name = "EN25S32",
		.nr_sectors_shift = 10,
		.sector_size_kib_shift = 2,
	},
	{
		.id = EON_ID_EN25S64,
		.name = "EN25S64",
		.nr_sectors_shift = 11,
		.sector_size_kib_shift = 2,
	},
};

static const struct spi_flash_ops spi_flash_ops = {
	.read = spi_flash_cmd_read,
	.write = spi_flash_cmd_write_page_program,
	.erase = spi_flash_cmd_erase,
	.status = spi_flash_cmd_status,
};

int spi_flash_probe_eon(const struct spi_slave *spi, u8 *idcode,
			struct spi_flash *flash)
{
	const struct spi_flash_part_id *params;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(flash_table); ++i) {
		params = &flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported EON ID %#02x%02x\n",
		       idcode[1], idcode[2]);
		return -1;
	}

	memcpy(&flash->spi, spi, sizeof(*spi));

	flash->name = params->name;
	flash->page_size = 256;
	flash->sector_size = (1U << params->sector_size_kib_shift) * KiB;
	flash->size = flash->sector_size * (1U << params->nr_sectors_shift);
	flash->erase_cmd = CMD_EN25_SE;
	flash->status_cmd = CMD_EN25_RDSR;
	flash->pp_cmd = CMD_EN25_PP;
	flash->wren_cmd = CMD_EN25_WREN;

	flash->ops = &spi_flash_ops;

	return 0;
}
