/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

/* this is a spi driver which adapts emmc to fake spi flash */

#include <soc/flash_controller.h>
#include <spi_flash.h>

static void init_io(void)
{
}

static int emmc_adapter_read(const struct spi_flash *flash, u32 addr,
		size_t len, void *buf)
{
	return 0;
}

static int emmc_adapter_write(const struct spi_flash *flash, u32 addr,
		size_t len, const void *buf)
{
	return 0;
}

static int emmc_adapter_erase(const struct spi_flash *flash, u32 offset,
		size_t len)
{
	return 0;
}

const struct spi_flash_ops spi_emmc_flash_ops = {
	.read = emmc_adapter_read,
	.write = emmc_adapter_write,
	.erase = emmc_adapter_erase,
};

int mtk_spi_flash_probe(const struct spi_slave *spi, struct spi_flash *flash)
{
	init_io();

	flash->name = "spi emmc flash controller";
	flash->sector_size = 0x800;
	flash->size = CONFIG_ROM_SIZE;

	flash->ops = &spi_emmc_flash_ops;
	return 0;
}
