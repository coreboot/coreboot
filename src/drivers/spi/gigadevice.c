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

/* GD25Pxx-specific commands */
#define CMD_GD25_WREN		0x06	/* Write Enable */
#define CMD_GD25_WRDI		0x04	/* Write Disable */
#define CMD_GD25_RDSR		0x05	/* Read Status Register */
#define CMD_GD25_WRSR		0x01	/* Write Status Register */
#define CMD_GD25_READ		0x03	/* Read Data Bytes */
#define CMD_GD25_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_GD25_PP		0x02	/* Page Program */
#define CMD_GD25_SE		0x20	/* Sector (4K) Erase */
#define CMD_GD25_BE		0xd8	/* Block (64K) Erase */
#define CMD_GD25_CE		0xc7	/* Chip Erase */
#define CMD_GD25_DP		0xb9	/* Deep Power-down */
#define CMD_GD25_RES		0xab	/* Release from DP, and Read Signature */

static const struct spi_flash_part_id flash_table[] = {
	{
		.id				= 0x3114,
		.name				= "GD25T80",
		.nr_sectors_shift		= 8,
		.sector_size_kib_shift		= 2,
	},
	{
		.id				= 0x4014,
		.name				= "GD25Q80",
		.nr_sectors_shift		= 8,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q80B */
	{
		.id				= 0x4015,
		.name				= "GD25Q16",
		.nr_sectors_shift		= 9,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q16B */
	{
		.id				= 0x4016,
		.name				= "GD25Q32B",
		.nr_sectors_shift		= 10,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q32B */
	{
		.id				= 0x4017,
		.name				= "GD25Q64",
		.nr_sectors_shift		= 11,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q64B, GD25B64C */
	{
		.id				= 0x4018,
		.name				= "GD25Q128",
		.nr_sectors_shift		= 12,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q128B */
	{
		.id				= 0x4214,
		.name				= "GD25VQ80C",
		.nr_sectors_shift		= 8,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},
	{
		.id				= 0x4215,
		.name				= "GD25VQ16C",
		.nr_sectors_shift		= 9,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},
	{
		.id				= 0x6014,
		.name				= "GD25LQ80",
		.nr_sectors_shift		= 8,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},
	{
		.id				= 0x6015,
		.name				= "GD25LQ16",
		.nr_sectors_shift		= 9,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},
	{
		.id				= 0x6016,
		.name				= "GD25LQ32",
		.nr_sectors_shift		= 10,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},
	{
		.id				= 0x6017,
		.name				= "GD25LQ64C",
		.nr_sectors_shift		= 11,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25LB64C */
	{
		.id				= 0x6018,
		.name				= "GD25LQ128",
		.nr_sectors_shift		= 12,
		.sector_size_kib_shift		= 2,
		.fast_read_dual_output_support	= 1,
	},
};

static const struct spi_flash_ops spi_flash_ops = {
	.read = spi_flash_cmd_read,
	.write = spi_flash_cmd_write_page_program,
	.erase = spi_flash_cmd_erase,
	.status = spi_flash_cmd_status,
};

int spi_flash_probe_gigadevice(const struct spi_slave *spi, u8 *idcode,
				struct spi_flash *flash)
{
	const struct spi_flash_part_id *params;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(flash_table); i++) {
		params = &flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(flash_table)) {
		printk(BIOS_WARNING,
		       "SF gigadevice.c: Unsupported ID %#02x%02x\n",
		       idcode[1], idcode[2]);
		return -1;
	}

	memcpy(&flash->spi, spi, sizeof(*spi));
	flash->name = params->name;

	/* Assuming power-of-two page size initially. */
	flash->page_size = 256;
	flash->sector_size = (1U << params->sector_size_kib_shift) * KiB;
	flash->size = flash->sector_size * (1U << params->nr_sectors_shift);
	flash->erase_cmd = CMD_GD25_SE;
	flash->status_cmd = CMD_GD25_RDSR;
	flash->pp_cmd = CMD_GD25_PP;
	flash->wren_cmd = CMD_GD25_WREN;

	flash->ops = &spi_flash_ops;

	return 0;
}
