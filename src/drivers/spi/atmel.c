/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

#include "spi_flash_internal.h"

/* M25Pxx-specific commands */
#define CMD_AT25_WREN		0x06	/* Write Enable */
#define CMD_AT25_WRDI		0x04	/* Write Disable */
#define CMD_AT25_RDSR		0x05	/* Read Status Register */
#define CMD_AT25_WRSR		0x01	/* Write Status Register */
#define CMD_AT25_READ		0x03	/* Read Data Bytes */
#define CMD_AT25_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_AT25_PP			0x02	/* Page Program */
#define CMD_AT25_SE			0x20	/* Sector (4K) Erase */
#define CMD_AT25_BE			0xd8	/* Block (64K) Erase */
#define CMD_AT25_CE			0xc7	/* Chip Erase */
#define CMD_AT25_DP			0xb9	/* Deep Power-down */
#define CMD_AT25_RES		0xab	/* Release from DP, and Read Signature */

static const struct spi_flash_part_id flash_table[] = {
	{
		/* AT25X16 */
		.id[0]			= 0x3015,
		.nr_sectors_shift	= 9,
	},
	{
		/* AT25DF32 */
		.id[0]			= 0x47,
		.nr_sectors_shift	= 10,
	},
	{
		/* AT25X64 */
		.id[0]			= 0x3017,
		.nr_sectors_shift	= 11,
	},
	{
		/* AT25Q16 */
		.id[0]			= 0x4015,
		.nr_sectors_shift	= 9,
	},
	{
		/* AT25Q32 */
		.id[0]			= 0x4016,
		.nr_sectors_shift	= 10,
	},
	{
		/* AT25Q64 */
		.id[0]			= 0x4017,
		.nr_sectors_shift	= 11,
	},
	{
		/* AT25Q128 */
		.id[0]			= 0x4018,
		.nr_sectors_shift	= 12,
	},
};

const struct spi_flash_vendor_info spi_flash_atmel_vi = {
	.id = VENDOR_ID_ATMEL,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
