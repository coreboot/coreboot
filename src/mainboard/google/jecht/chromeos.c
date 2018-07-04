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

#include <string.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/sata.h>
#include "onboard.h"

#define GPIO_SPI_WP	58
#define GPIO_REC_MODE	12

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1
#define FLAG_DEV_MODE	2

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_SPI_WP, ACTIVE_HIGH,
			get_gpio(GPIO_SPI_WP), "write protect"},
		{GPIO_REC_MODE, ACTIVE_LOW,
			get_recovery_mode_switch(), "recovery"},
		{-1, ACTIVE_HIGH, 1, "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}
#endif

int get_write_protect_state(void)
{
#ifdef __PRE_RAM__
	pci_devfn_t dev;
	dev = PCI_DEV(0, 0x1f, 2);
#else
	struct device *dev;
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_SPI_WP) & 1;
}

int get_recovery_mode_switch(void)
{
#ifdef __PRE_RAM__
	pci_devfn_t dev;
	dev = PCI_DEV(0, 0x1f, 2);
#else
	struct device *dev;
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_REC_MODE) & 1;
}

#ifdef __PRE_RAM__
void save_chromeos_gpios(void)
{
	u32 flags = 0;

	/* Write Protect: GPIO58 = GPIO_SPI_WP, active high */
	if (get_gpio(GPIO_SPI_WP))
		flags |= (1 << FLAG_SPI_WP);

	/* Recovery: GPIO12 = RECOVERY_L, active low */
	if (!get_gpio(GPIO_REC_MODE))
		flags |= (1 << FLAG_REC_MODE);

	/* Developer: Virtual */

	pci_write_config32(PCI_DEV(0, 0x1f, 2), SATA_SP, flags);
}
#endif

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
