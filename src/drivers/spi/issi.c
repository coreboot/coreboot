/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <delay.h>

#include "spi_flash_internal.h"

static const struct spi_flash_part_id flash_table[] = {
	{
		/* IS25WP256D */
		.id[0]				= 0x7019,
		.nr_sectors_shift		= 13,
	},
};

const struct spi_flash_vendor_info spi_flash_issi_vi = {
	.id = VENDOR_ID_ISSI,
	.page_size_shift = 8,        // 256 byte page size
	.sector_size_kib_shift = 2,  // 4 Kbyte sector size
	.match_id_mask[0] = 0xffff,
	.ids = flash_table,
	.nr_part_ids = ARRAY_SIZE(flash_table),
	.desc = &spi_flash_pp_0x20_sector_desc,
	.prot_ops = NULL,
};
