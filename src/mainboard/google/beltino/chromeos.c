/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

#include <device/pci_ops.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define GPIO_SPI_WP	58
#define GPIO_REC_MODE	12

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_SPI_WP, ACTIVE_HIGH,
			get_write_protect_state(), "write protect"},
		{GPIO_REC_MODE, ACTIVE_LOW,
			!get_recovery_mode_switch(), "presence"},
		{-1, ACTIVE_HIGH, 1, "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 2);
	return (pci_s_read_config32(dev, SATA_SP) >> FLAG_SPI_WP) & 1;
}

int get_recovery_mode_switch(void)
{
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 2);
	return (pci_s_read_config32(dev, SATA_SP) >> FLAG_REC_MODE) & 1;
}

void init_bootmode_straps(void)
{
	u32 flags = 0;
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 2);

	/* Write Protect: GPIO58 = GPIO_SPI_WP, active high */
	if (get_gpio(GPIO_SPI_WP))
		flags |= (1 << FLAG_SPI_WP);

	/* Recovery: GPIO12 = RECOVERY_L, active low */
	if (!get_gpio(GPIO_REC_MODE))
		flags |= (1 << FLAG_REC_MODE);

	/* Developer: Virtual */

	pci_s_write_config32(dev, SATA_SP, flags);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
