/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <stdint.h>
#include <stddef.h>
#include <console/console.h>
#include <string.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <soc/spi.h>
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif
#include "nvm.h"

/* This module assumes the flash is memory mapped just below 4GiB in the
 * address space for reading. Also this module assumes an area it erased
 * when all bytes read as all 0xff's. */

static struct spi_flash *flash;

static int nvm_init(void)
{
	if (flash != NULL)
		return 0;

	spi_init();
	flash = spi_flash_probe(0, 0);
	if (!flash) {
		printk(BIOS_DEBUG, "Could not find SPI device\n");
		return -1;
	}

	return 0;
}

/* Convert memory mapped pointer to flash offset. */
static inline uint32_t to_flash_offset(void *p)
{
	return CONFIG_ROM_SIZE + (uintptr_t)p;
}

int nvm_is_erased(const void *start, size_t size)
{
	const uint8_t *cur = start;
	const uint8_t erased_value = 0xff;

	while (size > 0) {
		if (*cur != erased_value)
			return 0;
		cur++;
		size--;
	}
	return 1;
}

int nvm_erase(void *start, size_t size)
{
	if (nvm_init() < 0)
		return -1;
	return flash->erase(flash, to_flash_offset(start), size);
}

/* Write data to NVM. Returns 0 on success < 0 on error.  */
int nvm_write(void *start, const void *data, size_t size)
{
	if (nvm_init() < 0)
		return -1;
	return flash->write(flash, to_flash_offset(start), size, data);
}

/* Read flash status register to determine if write protect is active */
int nvm_is_write_protected(void)
{
	u8 sr1;
	u8 wp_gpio = 0;
	u8 wp_spi;

	if (nvm_init() < 0)
		return -1;

#if IS_ENABLED(CONFIG_CHROMEOS)
	/* Read Write Protect GPIO if available */
	wp_gpio = get_write_protect_state();
#endif

	/* Read Status Register 1 */
	if (flash->status(flash, &sr1) < 0) {
		printk(BIOS_ERR, "Failed to read SPI status register 1\n");
		return -1;
	}
	wp_spi = !!(sr1 & 0x80);

	printk(BIOS_DEBUG, "SPI flash protection: WPSW=%d SRP0=%d\n",
	       wp_gpio, wp_spi);

	return wp_gpio && wp_spi;
}

/* Apply protection to a range of flash */
int nvm_protect(void *start, size_t size)
{
#if IS_ENABLED(CONFIG_MRC_SETTINGS_PROTECT)
	if (nvm_init() < 0)
		return -1;
	return spi_flash_protect(to_flash_offset(start), size);
#else
	return -1;
#endif
}
