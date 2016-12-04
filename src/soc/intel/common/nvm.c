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
#include <bootmode.h>
#include <console/console.h>
#include <string.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "nvm.h"
#include "spi_flash.h"

/* Read flash status register to determine if write protect is active */
int nvm_is_write_protected(void)
{
	u8 sr1;
	u8 wp_gpio;
	u8 wp_spi;

	if (!IS_ENABLED(CONFIG_CHROMEOS))
		return 0;

	if (!IS_ENABLED(CONFIG_BOOT_DEVICE_SPI_FLASH))
		return 0;

	/* Read Write Protect GPIO if available */
	wp_gpio = get_write_protect_state();

	/* Read Status Register 1 */
	if (spi_flash_status(boot_device_spi_flash(), &sr1) < 0) {
		printk(BIOS_ERR, "Failed to read SPI status register 1\n");
		return -1;
	}
	wp_spi = !!(sr1 & 0x80);

	printk(BIOS_DEBUG, "SPI flash protection: WPSW=%d SRP0=%d\n",
		wp_gpio, wp_spi);

	return wp_gpio && wp_spi;
}

/* Apply protection to a range of flash */
int nvm_protect(const struct region *r)
{
	if (!IS_ENABLED(CONFIG_MRC_SETTINGS_PROTECT))
		return 0;

	if (!IS_ENABLED(CONFIG_BOOT_DEVICE_SPI_FLASH))
		return 0;

	return spi_flash_protect(region_offset(r), region_sz(r));
}
