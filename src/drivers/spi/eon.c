/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

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
		/* EN25B80 */
		.id[0] = EON_ID_EN25B80,
		.nr_sectors_shift = 8,
	},
	{
		/* EN25B16 */
		.id[0] = EON_ID_EN25B16,
		.nr_sectors_shift = 9,
	},
	{
		/* EN25B32 */
		.id[0] = EON_ID_EN25B32,
		.nr_sectors_shift = 10,
	},
	{
		/* EN25B64 */
		.id[0] = EON_ID_EN25B64,
		.nr_sectors_shift = 11,
	},
	{
		/* EN25F80 */
		.id[0] = EON_ID_EN25F80,
		.nr_sectors_shift = 8,
	},
	{
		/* EN25F16 */
		.id[0] = EON_ID_EN25F16,
		.nr_sectors_shift = 9,
	},
	{
		/* EN25F32 */
		.id[0] = EON_ID_EN25F32,
		.nr_sectors_shift = 10,
	},
	{
		/* EN25F64 */
		.id[0] = EON_ID_EN25F64,
		.nr_sectors_shift = 11,
	},
	{
		/* EN25Q80(A) */
		.id[0] = EON_ID_EN25Q80,
		.nr_sectors_shift = 8,
	},
	{
		/* EN25Q16(D16) */
		.id[0] = EON_ID_EN25Q16,
		.nr_sectors_shift = 9,
	},
	{
		/* EN25Q32(A/B) */
		.id[0] = EON_ID_EN25Q32,
		.nr_sectors_shift = 10,
	},
	{
		/* EN25Q64 */
		.id[0] = EON_ID_EN25Q64,
		.nr_sectors_shift = 11,
	},
	{
		/* EN25Q128 */
		.id[0] = EON_ID_EN25Q128,
		.nr_sectors_shift = 12,
	},
	{
		/* EN25QH16 */
		.id[0] = EON_ID_EN25QH16,
		.nr_sectors_shift = 9,
	},
	{
		/* EN25QH32 */
		.id[0] = EON_ID_EN25QH32,
		.nr_sectors_shift = 10,
	},
	{
		/* EN25QH64 */
		.id[0] = EON_ID_EN25QH64,
		.nr_sectors_shift = 11,
	},
	{
		/* EN25QH128 */
		.id[0] = EON_ID_EN25QH128,
		.nr_sectors_shift = 12,
	},
	{
		/* EN25S80 */
		.id[0] = EON_ID_EN25S80,
		.nr_sectors_shift = 8,
	},
	{
		/* EN25S16 */
		.id[0] = EON_ID_EN25S16,
		.nr_sectors_shift = 9,
	},
	{
		/* EN25S32 */
		.id[0] = EON_ID_EN25S32,
		.nr_sectors_shift = 10,
	},
	{
		/* EN25S64 */
		.id[0] = EON_ID_EN25S64,
		.nr_sectors_shift = 11,
	},
};

const struct spi_flash_vendor_info spi_flash_eon_vi = {
	.id = VENDOR_ID_EON,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
