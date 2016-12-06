/*
 * Copyright (C) 2014 Idwer Vollering <vidwer@gmail.com>
 *
 * Based on winbond.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Author: Jason McMullan <mcmullan <at> netapp.com>
 * Licensed under the GPL-2 or later.
 */

#include <console/console.h>
#include <stdlib.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <string.h>

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

struct amic_spi_flash_params {
	uint16_t	id;
	/* Log2 of page size in power-of-two mode */
	uint8_t		l2_page_size;
	uint16_t	pages_per_sector;
	uint16_t	sectors_per_block;
	uint16_t	nr_blocks;
	const char	*name;
};

/* spi_flash needs to be first so upper layers can free() it */
struct amic_spi_flash {
	struct spi_flash flash;
	const struct amic_spi_flash_params *params;
};

static inline struct amic_spi_flash *
to_amic_spi_flash(const struct spi_flash *flash)
{
	return container_of(flash, struct amic_spi_flash, flash);
}

static const struct amic_spi_flash_params amic_spi_flash_table[] = {
	{
		.id			= 0x3016,
		.l2_page_size		= 8,
		.pages_per_sector	= 16,
		.sectors_per_block	= 16,
		.nr_blocks		= 64,
		.name			= "A25L032",
	},
};

static int amic_write(const struct spi_flash *flash, u32 offset, size_t len,
		const void *buf)
{
	struct amic_spi_flash *amic = to_amic_spi_flash(flash);
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret;
	u8 cmd[4];

	page_size = 1 << amic->params->l2_page_size;
	byte_addr = offset % page_size;

	for (actual = 0; actual < len; actual += chunk_len) {
		chunk_len = min(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(sizeof(cmd), chunk_len);

		cmd[0] = CMD_A25_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;
#if CONFIG_DEBUG_SPI_FLASH
		printk(BIOS_SPEW, "PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x }"
		        " chunk_len = %zu\n", buf + actual,
			cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd(&flash->spi, CMD_A25_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			goto out;
		}

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
				buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: AMIC Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret)
			goto out;

		offset += chunk_len;
		byte_addr = 0;
	}

#if CONFIG_DEBUG_SPI_FLASH
	printk(BIOS_SPEW, "SF: AMIC: Successfully programmed %zu bytes @"
			" 0x%lx\n", len, (unsigned long)(offset - len));
#endif
	ret = 0;

out:
	return ret;
}

struct spi_flash *spi_flash_probe_amic(struct spi_slave *spi, u8 *idcode)
{
	const struct amic_spi_flash_params *params;
	unsigned page_size;
	struct amic_spi_flash *amic;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(amic_spi_flash_table); i++) {
		params = &amic_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(amic_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported AMIC ID %02x%02x\n",
				idcode[1], idcode[2]);
		return NULL;
	}

	amic = malloc(sizeof(struct amic_spi_flash));
	if (!amic) {
		printk(BIOS_WARNING, "SF: Failed to allocate memory\n");
		return NULL;
	}

	amic->params = params;
	memcpy(&amic->flash.spi, spi, sizeof(*spi));
	amic->flash.name = params->name;

	/* Assuming power-of-two page size initially. */
	page_size = 1 << params->l2_page_size;

	amic->flash.internal_write = amic_write;
	amic->flash.internal_erase = spi_flash_cmd_erase;
#if CONFIG_SPI_FLASH_NO_FAST_READ
	amic->flash.internal_read = spi_flash_cmd_read_slow;
#else
	amic->flash.internal_read = spi_flash_cmd_read_fast;
#endif
	amic->flash.sector_size = (1 << amic->params->l2_page_size) *
		amic->params->pages_per_sector;
	amic->flash.size = page_size * params->pages_per_sector
				* params->sectors_per_block
				* params->nr_blocks;
	amic->flash.erase_cmd = CMD_A25_SE;

	return &amic->flash;
}
