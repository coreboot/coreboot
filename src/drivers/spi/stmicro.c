/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

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

static const struct spi_flash_part_id flash_table_se32k[] = {
	{
		/* M25P10 */
		.id[0] = STM_ID_M25P10,
		.nr_sectors_shift = 2,
	},
};

static const struct spi_flash_part_id flash_table_se64k[] = {
	{
		/* M25P16 */
		.id[0] = STM_ID_M25P16,
		.nr_sectors_shift = 5,
	},
	{
		/* M25P20 */
		.id[0] = STM_ID_M25P20,
		.nr_sectors_shift = 2,
	},
	{
		/* M25P32 */
		.id[0] = STM_ID_M25P32,
		.nr_sectors_shift = 6,
	},
	{
		/* M25P40 */
		.id[0] = STM_ID_M25P40,
		.nr_sectors_shift = 3,
	},
	{
		/* M25P64 */
		.id[0] = STM_ID_M25P64,
		.nr_sectors_shift = 7,
	},
	{
		/* M25P80 */
		.id[0] = STM_ID_M25P80,
		.nr_sectors_shift = 4,
	},
	{
		/* M25PX80 */
		.id[0] = STM_ID_M25PX80,
		.nr_sectors_shift = 4,
	},
	{
		/* M25PX16 */
		.id[0] = STM_ID_M25PX16,
		.nr_sectors_shift = 5,
	},
	{
		/* M25PX32 */
		.id[0] = STM_ID_M25PX32,
		.nr_sectors_shift = 6,
	},
	{
		/* M25PX64 */
		.id[0] = STM_ID_M25PX64,
		.nr_sectors_shift = 7,
	},
	{
		/* M25PE80 */
		.id[0] = STM_ID_M25PE80,
		.nr_sectors_shift = 4,
	},
	{
		/* M25PE16 */
		.id[0] = STM_ID_M25PE16,
		.nr_sectors_shift = 5,
	},
	{
		/* M25PE32 */
		.id[0] = STM_ID_M25PE32,
		.nr_sectors_shift = 6,
	},
	{
		/* M25PE64 */
		.id[0] = STM_ID_M25PE64,
		.nr_sectors_shift = 7,
	},
};

static const struct spi_flash_part_id flash_table_se256k[] = {
	{
		/* M25P128 */
		.id[0] = STM_ID_M25P128,
		.nr_sectors_shift = 6,
	},
};

static const struct spi_flash_part_id flash_table_sse[] = {
	{
		/* N25Q016..3E */
		.id[0] = STM_ID_N25Q016__3E,
		.nr_sectors_shift = 9,
	},
	{
		/* N25Q032..3E */
		.id[0] = STM_ID_N25Q032__3E,
		.nr_sectors_shift = 10,
	},
	{
		/* N25Q064..3E */
		.id[0] = STM_ID_N25Q064__3E,
		.nr_sectors_shift = 11,
	},
	{
		/* N25Q128..3E */
		.id[0] = STM_ID_N25Q128__3E,
		.nr_sectors_shift = 12,
	},
	{
		/* N25Q256..3E */
		.id[0] = STM_ID_N25Q256__3E,
		.nr_sectors_shift = 13,
	},
	{
		/* N25Q016..1E */
		.id[0] = STM_ID_N25Q016__1E,
		.nr_sectors_shift = 9,
	},
	{
		/* N25Q032..1E */
		.id[0] = STM_ID_N25Q032__1E,
		.nr_sectors_shift = 10,
	},
	{
		/* N25Q064..1E */
		.id[0] = STM_ID_N25Q064__1E,
		.nr_sectors_shift = 11,
	},
	{
		/* N25Q128..1E */
		.id[0] = STM_ID_N25Q128__1E,
		.nr_sectors_shift = 12,
	},
	{
		/* N25Q256..1E */
		.id[0] = STM_ID_N25Q256__1E,
		.nr_sectors_shift = 13,
	},
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

const struct spi_flash_vendor_info spi_flash_stmicro1_vi = {
	.id = VENDOR_ID_STMICRO,
	.page_size_shift = 8,
	.sector_size_kib_shift = 5,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table_se32k,
	.nr_part_ids = ARRAY_SIZE(flash_table_se32k),
	.desc = &spi_flash_pp_0xd8_sector_desc,
};

const struct spi_flash_vendor_info spi_flash_stmicro2_vi = {
	.id = VENDOR_ID_STMICRO,
	.page_size_shift = 8,
	.sector_size_kib_shift = 6,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table_se64k,
	.nr_part_ids = ARRAY_SIZE(flash_table_se64k),
	.desc = &spi_flash_pp_0xd8_sector_desc,
};

const struct spi_flash_vendor_info spi_flash_stmicro3_vi = {
	.id = VENDOR_ID_STMICRO,
	.page_size_shift = 8,
	.sector_size_kib_shift = 8,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table_se256k,
	.nr_part_ids = ARRAY_SIZE(flash_table_se256k),
	.desc = &spi_flash_pp_0xd8_sector_desc,
};

const struct spi_flash_vendor_info spi_flash_stmicro4_vi = {
	.id = VENDOR_ID_STMICRO,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table_sse,
	.nr_part_ids = ARRAY_SIZE(flash_table_sse),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
