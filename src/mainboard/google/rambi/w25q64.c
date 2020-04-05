/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <string.h>
#include <soc/spi.h>

/*
 * SPI lockdown configuration W25Q64FW.
 */
#define SPI_OPMENU_0 0x01 /* WRSR: Write Status Register */
#define SPI_OPTYPE_0 0x01 /* Write, no address */

#define SPI_OPMENU_1 0x02 /* BYPR: Byte Program */
#define SPI_OPTYPE_1 0x03 /* Write, address required */

#define SPI_OPMENU_2 0x03 /* READ: Read Data */
#define SPI_OPTYPE_2 0x02 /* Read, address required */

#define SPI_OPMENU_3 0x05 /* RDSR: Read Status Register */
#define SPI_OPTYPE_3 0x00 /* Read, no address */

#define SPI_OPMENU_4 0x20 /* SE20: Sector Erase 0x20 */
#define SPI_OPTYPE_4 0x03 /* Write, address required */

#define SPI_OPMENU_5 0x9f /* RDID: Read ID */
#define SPI_OPTYPE_5 0x00 /* Read, no address */

#define SPI_OPMENU_6 0xd8 /* BED8: Block Erase 0xd8 */
#define SPI_OPTYPE_6 0x03 /* Write, address required */

#define SPI_OPMENU_7 0x0b /* FAST: Fast Read */
#define SPI_OPTYPE_7 0x02 /* Read, address required */

#define SPI_OPPREFIX ((0x50 << 8) | 0x06) /* EWSR and WREN */
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
	.preop = SPI_OPPREFIX,
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
