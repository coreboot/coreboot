/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2018 Eltan B.V.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/spi.h>
#include <string.h>
#include <drivers/spi/spi_winbond.h>

/*
 * SPI lockdown configuration
 */
#define SPI_OPMENU_0	CMD_W25_WRSR		/* Write Status Register */
#define SPI_OPTYPE_0	SPI_OPTYPE_WR_NOADDR	/* Write, no address */

#define SPI_OPMENU_1	CMD_W25_PP		/* BYPR: Byte Program */
#define SPI_OPTYPE_1	SPI_OPTYPE_WR_ADDR	/* Write, address required */

#define SPI_OPMENU_2	CMD_W25_READ		/* Read Data */
#define SPI_OPTYPE_2	SPI_OPTYPE_RD_ADDR	/* Read, address required */

#define SPI_OPMENU_3	CMD_W25_RDSR		/* Read Status Register */
#define SPI_OPTYPE_3	SPI_OPTYPE_RD_NOADDR	/* Read, no address */

#define SPI_OPMENU_4	CMD_W25_SE		/* Sector Erase */
#define SPI_OPTYPE_4	SPI_OPTYPE_WR_ADDR	/* Write, address required */

#define SPI_OPMENU_5	CMD_W25_RDID		/* Read ID */
#define SPI_OPTYPE_5	SPI_OPTYPE_RD_NOADDR	/* Read, no address */

#define SPI_OPMENU_6	CMD_W25_BE		/* BE: Block Erase */
#define SPI_OPTYPE_6	SPI_OPTYPE_WR_ADDR	/* Write, address required */

#define SPI_OPMENU_7	CMD_W25_FAST_READ	/* FAST: Fast Read */
#define SPI_OPTYPE_7	SPI_OPTYPE_RD_ADDR	/* Read, address required */

#define SPI_OPPREFIX	CMD_W25_WREN		/* WREN only to be inline */
						/* with flashrom */

#define SPI_OPTYPE ((SPI_OPTYPE_7 << 14) | (SPI_OPTYPE_6 << 12) | \
		    (SPI_OPTYPE_5 << 10) | (SPI_OPTYPE_4 <<  8) | \
		    (SPI_OPTYPE_3 <<  6) | (SPI_OPTYPE_2 <<  4) | \
		    (SPI_OPTYPE_1 <<  2) | (SPI_OPTYPE_0 <<  0))

#define SPI_OPMENU_UPPER ((SPI_OPMENU_7 << 24) | (SPI_OPMENU_6 << 16) | \
			  (SPI_OPMENU_5 <<  8) | (SPI_OPMENU_4 <<  0))

#define SPI_OPMENU_LOWER ((SPI_OPMENU_3 << 24) | (SPI_OPMENU_2 << 16) | \
			  (SPI_OPMENU_1 <<  8) | (SPI_OPMENU_0 <<  0))

#define SPI_VSCC (WG_64_BYTE | EO(0x20) | BES_4_KB)

static const struct spi_config spi_config = {
	.preop = CMD_W25_WREN,
	.optype = SPI_OPTYPE,
	.opmenu = { SPI_OPMENU_LOWER, SPI_OPMENU_UPPER },
	.lvscc =  SPI_VSCC,
	.uvscc =  SPI_VSCC,
};

int mainboard_get_spi_config(struct spi_config *cfg)
{
	memcpy(cfg, &spi_config, sizeof(*cfg));

	return 0;
}
