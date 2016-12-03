/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <arch/early_variables.h>
#include <boot_device.h>
#include <spi_flash.h>
#include <spi-generic.h>

static struct spi_flash *sfg CAR_GLOBAL;

static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	struct spi_flash *sf = car_get_var(sfg);

	if (sf == NULL)
		return -1;

	if (spi_flash_read(sf, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	struct spi_flash *sf = car_get_var(sfg);

	if (sf == NULL)
		return -1;

	if (spi_flash_write(sf, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	struct spi_flash *sf = car_get_var(sfg);

	if (sf == NULL)
		return -1;

	if (spi_flash_erase(sf, offset, size))
		return -1;

	return size;
}

static const struct region_device_ops spi_ops = {
	.readat = spi_readat,
	.writeat = spi_writeat,
	.eraseat = spi_eraseat,
};

static const struct region_device spi_rw =
	REGION_DEV_INIT(&spi_ops, 0, CONFIG_ROM_SIZE);

static void boot_device_rw_init(void)
{
	const int bus = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS;
	const int cs = 0;

	if (car_get_var(sfg) != NULL)
		return;

	/* Ensure any necessary setup is performed by the drivers. */
	spi_init();

	car_set_var(sfg, spi_flash_probe(bus, cs));
}

const struct region_device *boot_device_rw(void)
{
	/* Probe for the SPI flash device if not already done. */
	boot_device_rw_init();

	if (car_get_var(sfg) == NULL)
		return NULL;

	return &spi_rw;
}

const struct spi_flash *boot_device_spi_flash(void)
{
	boot_device_rw_init();
	return car_get_var(sfg);
}
