/*
 * (C) Copyright 2010, ucRobotics Inc.
 * Copyright (c) 2014, Sage Electronic Engineering, LLC
 * .
 * Author: Chong Huang <chuang@ucrobotics.com>
 * Licensed under the GPL-2 or later.
 */

#include <console/console.h>
#include <stdlib.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <string.h>

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

#define EON_ID_EN25Q128		0x3018
#define EON_ID_EN25Q64		0x3017
#define EON_ID_EN25S64		0x3817

struct eon_spi_flash_params {
	u16 id;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_sectors;
	const char *name;
};

/* spi_flash needs to be first so upper layers can free() it */
struct eon_spi_flash {
	struct spi_flash flash;
	const struct eon_spi_flash_params *params;
};

static inline
struct eon_spi_flash *to_eon_spi_flash(const struct spi_flash *flash)
{
	return container_of(flash, struct eon_spi_flash, flash);
}

static const struct eon_spi_flash_params eon_spi_flash_table[] = {
	{
		.id = EON_ID_EN25Q128,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 4096,
		.name = "EN25Q128",
	},
	{
		.id = EON_ID_EN25Q64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25Q64",
	},
	{
		.id = EON_ID_EN25S64,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_sectors = 2048,
		.name = "EN25S64",
	},
};

static int eon_write(const struct spi_flash *flash,
		     u32 offset, size_t len, const void *buf)
{
	struct eon_spi_flash *eon = to_eon_spi_flash(flash);
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret = 0;
	u8 cmd[4];

	page_size = 1 << eon->params->page_size;
	byte_addr = offset % page_size;

	for (actual = 0; actual < len; actual += chunk_len) {
		chunk_len = min(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(sizeof(cmd), chunk_len);

		ret = spi_flash_cmd(&flash->spi, CMD_EN25_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			goto out;
		}

		cmd[0] = CMD_EN25_PP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;

#if CONFIG_DEBUG_SPI_FLASH
		printk(BIOS_SPEW,
		    "PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %zu\n",
		     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
					  buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: EON Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret) {
			printk(BIOS_WARNING, "SF: EON Page Program timeout\n");
			goto out;
		}

		offset += chunk_len;
		byte_addr = 0;
	}

#if CONFIG_DEBUG_SPI_FLASH
	printk(BIOS_SPEW, "SF: EON: Successfully programmed %zu bytes @ %#x\n",
	       len, (unsigned int)(offset - len));
#endif

out:
	return ret;
}

struct spi_flash *spi_flash_probe_eon(struct spi_slave *spi, u8 *idcode)
{
	const struct eon_spi_flash_params *params;
	struct eon_spi_flash *eon;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(eon_spi_flash_table); ++i) {
		params = &eon_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(eon_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported EON ID %#02x%02x\n",
		       idcode[1], idcode[2]);
		return NULL;
	}

	eon = malloc(sizeof(*eon));
	if (!eon) {
		printk(BIOS_WARNING, "SF: Failed to allocate memory\n");
		return NULL;
	}

	eon->params = params;
	memcpy(&eon->flash.spi, spi, sizeof(*spi));
	eon->flash.name = params->name;

	eon->flash.internal_write = eon_write;
	eon->flash.internal_erase = spi_flash_cmd_erase;
	eon->flash.internal_status = spi_flash_cmd_status;
	eon->flash.internal_read = spi_flash_cmd_read_fast;
	eon->flash.sector_size = params->page_size * params->pages_per_sector;
	eon->flash.size = params->page_size * params->pages_per_sector
	    * params->nr_sectors;
	eon->flash.erase_cmd = CMD_EN25_SE;
	eon->flash.status_cmd = CMD_EN25_RDSR;

	return &eon->flash;
}
