/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

#include "spi_flash_internal.h"

/* S25FLxx-specific commands */
#define CMD_S25FLXX_READ	0x03	/* Read Data Bytes */
#define CMD_S25FLXX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_S25FLXX_READID	0x90	/* Read Manufacture ID and Device ID */
#define CMD_S25FLXX_WREN	0x06	/* Write Enable */
#define CMD_S25FLXX_WRDI	0x04	/* Write Disable */
#define CMD_S25FLXX_RDSR	0x05	/* Read Status Register */
#define CMD_S25FLXX_WRSR	0x01	/* Write Status Register */
#define CMD_S25FLXX_PP		0x02	/* Page Program */
#define CMD_S25FLXX_SE		0xd8	/* Sector Erase */
#define CMD_S25FLXX_BE		0xc7	/* Bulk Erase */
#define CMD_S25FLXX_DP		0xb9	/* Deep Power-down */
#define CMD_S25FLXX_RES		0xab	/* Release from DP, and Read Signature */

#define SPSN_ID_S25FL008A	0x0213
#define SPSN_ID_S25FL016A	0x0214
#define SPSN_ID_S25FL032A	0x0215
#define SPSN_ID_S25FL064A	0x0216
#define SPSN_ID_S25FL128S	0x0219
#define SPSN_ID_S25FL128P	0x2018
#define SPSN_ID_S25FL208K	0x4014
#define SPSN_ID_S25FL116K	0x4015
#define SPSN_ID_S25FL132K	0x4016
#define SPSN_ID_S25FL164K	0x4017
#define SPSN_EXT_ID_S25FL128P_256KB	0x0300
#define SPSN_EXT_ID_S25FL128P_64KB	0x0301
#define SPSN_EXT_ID_S25FL032P		0x4d00
#define SPSN_EXT_ID_S25FLXXS_64KB	0x4d01

static const struct spi_flash_part_id flash_table_ext[] = {
	{
		/* S25FL008A */
		.id[0] = SPSN_ID_S25FL008A,
		.nr_sectors_shift = 4,
	},
	{
		/* S25FL016A */
		.id[0] = SPSN_ID_S25FL016A,
		.nr_sectors_shift = 5,
	},
	{
		/* S25FL032A */
		.id[0] = SPSN_ID_S25FL032A,
		.nr_sectors_shift = 6,
	},
	{
		/* S25FL064A */
		.id[0] = SPSN_ID_S25FL064A,
		.nr_sectors_shift = 7,
	},
	{
		/* S25FL128P_64K */
		.id[0] = SPSN_ID_S25FL128P,
		.id[1] = SPSN_EXT_ID_S25FL128P_64KB,
		.nr_sectors_shift = 8,
	},
	{
		/* S25FL128S_256K */
		.id[0] = SPSN_ID_S25FL128S,
		.id[1] = SPSN_EXT_ID_S25FLXXS_64KB,
		.nr_sectors_shift = 9,
	},
	{
		/* S25FL032P */
		.id[0] = SPSN_ID_S25FL032A,
		.id[1] = SPSN_EXT_ID_S25FL032P,
		.nr_sectors_shift = 6,
	},
	{
		/* S25FS128S */
		.id[0] = SPSN_ID_S25FL128P,
		.id[1] = SPSN_EXT_ID_S25FLXXS_64KB,
		.nr_sectors_shift = 8,
	},
};

static const struct spi_flash_part_id flash_table_256k_sector[] = {
	{
		/* S25FL128P_256K */
		.id[0] = SPSN_ID_S25FL128P,
		.id[1] = SPSN_EXT_ID_S25FL128P_256KB,
		.nr_sectors_shift = 6,
	},
};

static const struct spi_flash_part_id flash_table[] = {
	{
		/* S25FL208K */
		.id[0] = SPSN_ID_S25FL208K,
		.nr_sectors_shift = 4,
	},
	{
		/* S25FL116K_16M */
		.id[0] = SPSN_ID_S25FL116K,
		.nr_sectors_shift = 5,
	},
	{
		/* S25FL132K */
		.id[0] = SPSN_ID_S25FL132K,
		.nr_sectors_shift = 6,
	},
	{
		/* S25FL164K */
		.id[0] = SPSN_ID_S25FL164K,
		.nr_sectors_shift = 7,
	},
};

const struct spi_flash_vendor_info spi_flash_spansion_ext1_vi = {
	.id = VENDOR_ID_SPANSION,
	.page_size_shift = 8,
	.sector_size_kib_shift = 6,
	.match_id_mask[0] = 0xffff,
	.match_id_mask[1] = 0xffff,
	.ids = flash_table_ext,
	.nr_part_ids = ARRAY_SIZE(flash_table_ext),
	.desc = &spi_flash_pp_0xd8_sector_desc,
};

const struct spi_flash_vendor_info spi_flash_spansion_ext2_vi = {
	.id = VENDOR_ID_SPANSION,
	.page_size_shift = 8,
	.sector_size_kib_shift = 8,
	.match_id_mask[0] = 0xffff,
	.match_id_mask[1] = 0xffff,
	.ids = flash_table_256k_sector,
	.nr_part_ids = ARRAY_SIZE(flash_table_256k_sector),
	.desc = &spi_flash_pp_0xd8_sector_desc,
};

const struct spi_flash_vendor_info spi_flash_spansion_vi = {
	.id = VENDOR_ID_SPANSION,
	.page_size_shift = 8,
	.sector_size_kib_shift = 6,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0xd8_sector_desc,
};
