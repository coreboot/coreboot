/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

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
		/* GD25T80 */
		.id[0]				= 0x3114,
		.nr_sectors_shift		= 8,
	},
	{
		/* GD25Q80 */
		.id[0]				= 0x4014,
		.nr_sectors_shift		= 8,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q80B */
	{
		/* GD25Q16 */
		.id[0]				= 0x4015,
		.nr_sectors_shift		= 9,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q16B */
	{
		/* GD25Q32B */
		.id[0]				= 0x4016,
		.nr_sectors_shift		= 10,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q32B */
	{
		/* GD25Q64 */
		.id[0]				= 0x4017,
		.nr_sectors_shift		= 11,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q64B, GD25B64C */
	{
		/* GD25Q128 */
		.id[0]				= 0x4018,
		.nr_sectors_shift		= 12,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25Q128B */
	{
		/* GD25VQ80C */
		.id[0]				= 0x4214,
		.nr_sectors_shift		= 8,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* GD25VQ16C */
		.id[0]				= 0x4215,
		.nr_sectors_shift		= 9,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* GD25LQ80 */
		.id[0]				= 0x6014,
		.nr_sectors_shift		= 8,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* GD25LQ16 */
		.id[0]				= 0x6015,
		.nr_sectors_shift		= 9,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* GD25LQ32 */
		.id[0]				= 0x6016,
		.nr_sectors_shift		= 10,
		.fast_read_dual_output_support	= 1,
	},
	{
		/* GD25LQ64C */
		.id[0]				= 0x6017,
		.nr_sectors_shift		= 11,
		.fast_read_dual_output_support	= 1,
	},					/* also GD25LB64C */
	{
		/* GD25LQ128 */
		.id[0]				= 0x6018,
		.nr_sectors_shift		= 12,
		.fast_read_dual_output_support	= 1,
	},
};

const struct spi_flash_vendor_info spi_flash_gigadevice_vi = {
	.id = VENDOR_ID_GIGADEVICE,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
