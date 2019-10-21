/*
 * This file is part of the coreboot project.
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
#include <stdint.h>

static struct spi_flash sfg CAR_GLOBAL;
static bool sfg_init_done CAR_GLOBAL;

static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	struct spi_flash *sf = car_get_var_ptr(&sfg);

	if (sf == NULL)
		return -1;

	if (spi_flash_read(sf, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	struct spi_flash *sf = car_get_var_ptr(&sfg);

	if (sf == NULL)
		return -1;

	if (spi_flash_write(sf, offset, size, b))
		return -1;

	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	struct spi_flash *sf = car_get_var_ptr(&sfg);

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

	if (car_get_var(sfg_init_done) == true)
		return;

	/* Ensure any necessary setup is performed by the drivers. */
	spi_init();

	if (!spi_flash_probe(bus, cs, car_get_var_ptr(&sfg)))
		car_set_var(sfg_init_done, true);
}

const struct region_device *boot_device_rw(void)
{
	/* Probe for the SPI flash device if not already done. */
	boot_device_rw_init();

	if (car_get_var(sfg_init_done) != true)
		return NULL;

	return &spi_rw;
}

const struct spi_flash *boot_device_spi_flash(void)
{
	boot_device_rw_init();

	if (car_get_var(sfg_init_done) != true)
		return NULL;

	return car_get_var_ptr(&sfg);
}

int boot_device_wp_region(const struct region_device *rd,
			  const enum bootdev_prot_type type)
{
	uint32_t ctrlr_pr;

	/* Ensure boot device has been initialized at least once. */
	boot_device_init();

	const struct spi_flash *boot_dev = boot_device_spi_flash();

	if (boot_dev == NULL)
		return -1;

	if (type == MEDIA_WP) {
		if (spi_flash_is_write_protected(boot_dev,
					region_device_region(rd)) != 1) {
			return spi_flash_set_write_protected(boot_dev,
						region_device_region(rd), true,
						SPI_WRITE_PROTECTION_REBOOT);
		}

		/* Already write protected */
		return 0;
	}

	switch (type) {
	case CTRLR_WP:
		ctrlr_pr = WRITE_PROTECT;
		break;
	case CTRLR_RP:
		ctrlr_pr = READ_PROTECT;
		break;
	case CTRLR_RWP:
		ctrlr_pr = READ_WRITE_PROTECT;
		break;
	default:
		return -1;
	}

	return spi_flash_ctrlr_protect_region(boot_dev,
					region_device_region(rd), ctrlr_pr);
}
