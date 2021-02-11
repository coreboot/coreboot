/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

#include "spi_flash_internal.h"

/* A25L-specific commands */
#define CMD_A25_WREN		0x06	/* Write Enable */
#define CMD_A25_WRDI		0x04	/* Write Disable */
#define CMD_A25_RDSR		0x05	/* Read Status Register */
#define CMD_A25_WRSR		0x01	/* Write Status Register */
#define CMD_A25_READ		0x03	/* Read Data Bytes */
#define CMD_A25_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_A25_PP		0x02	/* Page Program */
#define CMD_A25_SE		0x20	/* Sector (4K) Erase */
#define CMD_A25_BE		0xd8	/* Block (64K) Erase */
#define CMD_A25_CE		0xc7	/* Chip Erase */
#define CMD_A25_DP		0xb9	/* Deep Power-down */
#define CMD_A25_RES		0xab	/* Release from DP, and Read Signature */

static const struct spi_flash_part_id flash_table[] = {
	{
		/* A25L16PU */
		.id[0]			= 0x2015,
		.nr_sectors_shift	= 9,
	},
	{
		/* A25L16PT */
		.id[0]			= 0x2025,
		.nr_sectors_shift	= 9,
	},
	{
		/* A25L080 */
		.id[0]			= 0x3014,
		.nr_sectors_shift	= 8,
	},
	{
		/* A25L016 */
		.id[0]			= 0x3015,
		.nr_sectors_shift	= 9,
	},
	{
		/* A25L032 */
		.id[0]			= 0x3016,
		.nr_sectors_shift	= 10,
	},
	{
		/* A25LQ080 */
		.id[0]			= 0x4014,
		.nr_sectors_shift	= 8,
	},
	{
		/* A25LQ16 */
		.id[0]			= 0x4015,
		.nr_sectors_shift	= 9,
	},
	{
		/* A25LQ032 */
		.id[0]			= 0x4016,
		.nr_sectors_shift	= 10,
	},
	{
		/* A25LQ64 */
		.id[0]			= 0x4017,
		.nr_sectors_shift	= 11,
	},
};

const struct spi_flash_vendor_info spi_flash_amic_vi = {
	.id = VENDOR_ID_AMIC,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
