/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>

#include "spi_flash_internal.h"

/* MX25xx-specific commands */
#define CMD_MX25XX_WREN		0x06	/* Write Enable */
#define CMD_MX25XX_WRDI		0x04	/* Write Disable */
#define CMD_MX25XX_RDSR		0x05	/* Read Status Register */
#define CMD_MX25XX_WRSR		0x01	/* Write Status Register */
#define CMD_MX25XX_READ		0x03	/* Read Data Bytes */
#define CMD_MX25XX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_MX25XX_PP		0x02	/* Page Program */
#define CMD_MX25XX_SE		0x20	/* Sector Erase */
#define CMD_MX25XX_BE		0xD8	/* Block Erase */
#define CMD_MX25XX_CE		0xc7	/* Chip Erase */
#define CMD_MX25XX_DP		0xb9	/* Deep Power-down */
#define CMD_MX25XX_RES		0xab	/* Release from DP, and Read Signature */

#define MACRONIX_SR_WIP		(1 << 0)	/* Write-in-Progress */

static const struct spi_flash_part_id flash_table[] = {
	{
		/* MX25L8005 */
		.id[0] = 0x2014,
		.nr_sectors_shift = 8,
	},
	{
		/* MX25L1605D */
		.id[0] = 0x2015,
		.nr_sectors_shift = 9,
	},
	{
		/* MX25L3205D */
		.id[0] = 0x2016,
		.nr_sectors_shift = 10,
	},
	{
		/* MX25L6405D */
		.id[0] = 0x2017,
		.nr_sectors_shift = 11,
	},
	{
		/* MX25L12805D */
		.id[0] = 0x2018,
		.nr_sectors_shift = 12,
	},
	{
		/* MX25L25635F */
		.id[0] = 0x2019,
		.nr_sectors_shift = 13,
	},
	{
		/* MX66L51235F */
		.id[0] = 0x201a,
		.nr_sectors_shift = 14,
	},
	{
		/* MX25L1635D */
		.id[0] = 0x2415,
		.nr_sectors_shift = 9,
	},
	/*
	 * NOTE: C225xx JEDEC IDs are basically useless because Macronix keeps
	 * reusing the same IDs for vastly different chips. 35E versions always
	 * seem to support Dual I/O but not Dual Output, while 35F versions seem
	 * to support both, so we only set Dual I/O here to improve our chances
	 * of compatibility. Since Macronix makes it impossible to search all
	 * different parts that it recklessly assigned the same IDs to, it's
	 * hard to know if there may be parts that don't even support Dual I/O
	 * with these IDs, though (or what we should do if there are).
	 */
	{
		/* MX25L1635E */
		.id[0] = 0x2515,
		.nr_sectors_shift = 9,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U8032E */
		.id[0] = 0x2534,
		.nr_sectors_shift = 8,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U1635E/MX25U1635F */
		.id[0] = 0x2535,
		.nr_sectors_shift = 9,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U3235E/MX25U3235F */
		.id[0] = 0x2536,
		.nr_sectors_shift = 10,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U6435E/MX25U6435F */
		.id[0] = 0x2537,
		.nr_sectors_shift = 11,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U12835F */
		.id[0] = 0x2538,
		.nr_sectors_shift = 12,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U25635F */
		.id[0] = 0x2539,
		.nr_sectors_shift = 13,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25U51235F */
		.id[0] = 0x253a,
		.nr_sectors_shift = 14,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25L12855E */
		.id[0] = 0x2618,
		.nr_sectors_shift = 12,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25L3235D/MX25L3225D/MX25L3236D/MX25L3237D */
		.id[0] = 0x5e16,
		.nr_sectors_shift = 10,
		.fast_read_dual_io_support = 1,
	},
	{
		/* MX25L6495F */
		.id[0] = 0x9517,
		.nr_sectors_shift = 11,
	},
	{
		/* MX77U25650F */
		.id[0] = 0x7539,
		.nr_sectors_shift = 13,
	},
};

const struct spi_flash_vendor_info spi_flash_macronix_vi = {
	.id = VENDOR_ID_MACRONIX,
	.page_size_shift = 8,
	.sector_size_kib_shift = 2,
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
};
