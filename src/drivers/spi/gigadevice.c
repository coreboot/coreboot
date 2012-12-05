/*
 * Copyright (c) 2012, Google Inc.
 *
 * Based on drivers/spi/winbond.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */


#include <stdlib.h>
#include <spi_flash.h>
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

struct gigadevice_spi_flash_params {
	uint16_t	id;
	/* Log2 of page size in power-of-two mode */
	uint8_t		l2_page_size;
	uint16_t	pages_per_sector;
	uint16_t	sectors_per_block;
	uint16_t	nr_blocks;
	const char	*name;
};

/* spi_flash needs to be first so upper layers can free() it */
struct gigadevice_spi_flash {
	struct spi_flash flash;
	const struct gigadevice_spi_flash_params *params;
};

static inline struct gigadevice_spi_flash *
to_gigadevice_spi_flash(struct spi_flash *flash)
{
	return container_of(flash, struct gigadevice_spi_flash, flash);
}

static const struct gigadevice_spi_flash_params gigadevice_spi_flash_table[] = {
	{
		.id			= 0x4014,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 16,
		.name			= "GD25Q80",
	},
	{
		.id			= 0x4015,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 32,
		.name			= "GD25Q16(B)",
	},
	{
		.id			= 0x4016,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 64,
		.name			= "GD25Q32(B)",
	},
	{
		.id			= 0x6016,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 64,
		.name			= "GD25LQ32",
	},
	{
		.id			= 0x4017,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 128,
		.name			= "GD25Q64(B)",
	},
	{
		.id			= 0x4018,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 256,
		.name			= "GD25Q128(B)",
	},
};

static int gigadevice_write(struct spi_flash *flash, u32 offset,
			    size_t len, const void *buf)
{
	struct gigadevice_spi_flash *stm = to_gigadevice_spi_flash(flash);
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret;
	u8 cmd[4];

	page_size = min(1 << stm->params->l2_page_size, CONTROLLER_PAGE_LIMIT);
	byte_addr = offset % page_size;

	flash->spi->rw = SPI_WRITE_FLAG;
	ret = spi_claim_bus(flash->spi);
	if (ret) {
		printk(BIOS_WARNING,
		       "SF gigadevice.c: Unable to claim SPI bus\n");
		return ret;
	}

	for (actual = 0; actual < len; actual += chunk_len) {
		chunk_len = min(len - actual, page_size - byte_addr);

		ret = spi_flash_cmd(flash->spi, CMD_GD25_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING,
			       "SF gigadevice.c: Enabling Write failed\n");
			goto out;
		}

		cmd[0] = CMD_GD25_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;
#if CONFIG_DEBUG_SPI_FLASH
		printk(BIOS_SPEW,
		       "PP gigadevice.c: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x }"
		       " chunk_len = %zu\n", buf + actual,
		       cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd_write(flash->spi, cmd, 4,
					  buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING,
			       "SF gigadevice.c: Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret)
			goto out;

		offset += chunk_len;
		byte_addr = 0;
	}

#if CONFIG_DEBUG_SPI_FLASH
	printk(BIOS_SPEW,
	       "SF gigadevice.c: Successfully programmed %zu bytes @ %#x\n",
	       len, (unsigned int)(offset - len));
#endif

	ret = 0;

out:
	spi_release_bus(flash->spi);
	return ret;
}

static int gigadevice_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	return spi_flash_cmd_erase(flash, CMD_GD25_SE, offset, len);
}

struct spi_flash *spi_flash_probe_gigadevice(struct spi_slave *spi, u8 *idcode)
{
	const struct gigadevice_spi_flash_params *params;
	unsigned page_size;
	struct gigadevice_spi_flash *stm;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(gigadevice_spi_flash_table); i++) {
		params = &gigadevice_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(gigadevice_spi_flash_table)) {
		printk(BIOS_WARNING,
		       "SF gigadevice.c: Unsupported ID %#02x%02x\n",
		       idcode[1], idcode[2]);
		return NULL;
	}

	stm = malloc(sizeof(struct gigadevice_spi_flash));
	if (!stm) {
		printk(BIOS_WARNING,
		       "SF gigadevice.c: Failed to allocate memory\n");
		return NULL;
	}

	stm->params = params;
	stm->flash.spi = spi;
	stm->flash.name = params->name;

	/* Assuming power-of-two page size initially. */
	page_size = 1 << params->l2_page_size;

	stm->flash.write = gigadevice_write;
	stm->flash.erase = gigadevice_erase;
#if CONFIG_SPI_FLASH_NO_FAST_READ
	stm->flash.read = spi_flash_cmd_read_slow;
#else
	stm->flash.read = spi_flash_cmd_read_fast;
#endif
	stm->flash.sector_size = (1 << stm->params->l2_page_size) *
		stm->params->pages_per_sector;
	stm->flash.size = page_size * params->pages_per_sector
				* params->sectors_per_block
				* params->nr_blocks;

	return &stm->flash;
}
