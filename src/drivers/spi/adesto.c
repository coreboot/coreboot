/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Driver for Adesto Technologies SPI flash
 * based on winbond.c
 */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

#include "spi_flash_internal.h"

/* at25dfxx-specific commands */
#define CMD_AT25DF_WREN		0x06	/* Write Enable */
#define CMD_AT25DF_WRDI		0x04	/* Write Disable */
#define CMD_AT25DF_RDSR		0x05	/* Read Status Register */
#define CMD_AT25DF_WRSR		0x01	/* Write Status Register */
#define CMD_AT25DF_READ		0x03	/* Read Data Bytes */
#define CMD_AT25DF_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_AT25DF_PP		0x02	/* Page Program */
#define CMD_AT25DF_SE		0x20	/* Sector (4K) Erase */
#define CMD_AT25DF_BE		0xd8	/* Block (64K) Erase */
#define CMD_AT25DF_CE		0xc7	/* Chip Erase */
#define CMD_AT25DF_DP		0xb9	/* Deep Power-down */
#define CMD_AT25DF_RES		0xab	/* Release from DP, and Read Signature */

static const struct spi_flash_part_id flash_table[] = {
	{
		/* AT25SL128A */
		.id[0]			= 0x4218,
		.nr_sectors_shift	= 12,
	},
	{
		/* AT25DF081A Yes, 81A id < 81 */
		.id[0]			= 0x4501,
		.nr_sectors_shift	= 8,
	},
	{
		/* AT25DF081 */
		.id[0]			= 0x4502,
		.nr_sectors_shift	= 8,
	},
	{
		/* AT25DF161 */
		.id[0]			= 0x4602,
		.nr_sectors_shift	= 9,
	},
	{
		/* AT25DL161 */
		.id[0]			= 0x4603,
		.nr_sectors_shift	= 9,
	},
	{
		/* AT25DF321 */
		.id[0]			= 0x4700,
		.nr_sectors_shift	= 10,
	},
	{
		/* AT25DF321A */
		.id[0]			= 0x4701,
		.nr_sectors_shift	= 10,
	},
	{
		/* AT25DF641 */
		.id[0]			= 0x4800,
		.nr_sectors_shift	= 11,
	},
	{
		/* AT25SF081 */
		.id[0]			= 0x8501,
		.nr_sectors_shift	= 8,
	},
	{
		/* AT25DQ161 */
		.id[0]			= 0x8600,
		.nr_sectors_shift	= 9,
	},
	{
		/* AT25SF161 */
		.id[0]			= 0x8601,
		.nr_sectors_shift	= 9,
	},
	{
		/* AT25DQ321 */
		.id[0]			= 0x8700,
		.nr_sectors_shift	= 10,
	},
};

const struct spi_flash_vendor_info spi_flash_adesto_vi = {
	.id = VENDOR_ID_ADESTO,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
