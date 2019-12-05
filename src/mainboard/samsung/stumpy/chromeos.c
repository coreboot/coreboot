/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define GPIO_SPI_WP	68
#define GPIO_REC_MODE	42

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1

void fill_lb_gpios(struct lb_gpios *gpios)
{
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
	u16 gen_pmcon_1 = pci_s_read_config32(dev, GEN_PMCON_1);

	struct lb_gpio chromeos_gpios[] = {
		/* Write Protect: GPIO68 = CHP3_SPI_WP */
		{GPIO_SPI_WP, ACTIVE_HIGH, get_write_protect_state(),
		 "write protect"},

		/* Recovery: GPIO42 = CHP3_REC_MODE# */
		{GPIO_REC_MODE, ACTIVE_LOW, !get_recovery_mode_switch(),
		 "presence"},

		/* Hard code the lid switch GPIO to open. */
		{100, ACTIVE_HIGH, 1, "lid"},

		/* Power Button */
		{101, ACTIVE_LOW, (gen_pmcon_1 >> 9) & 1, "power"},

		/* Did we load the VGA Option ROM? */
		/* -1 indicates that this is a pseudo GPIO */
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

	/* Write Protect: GPIO68 = CHP3_SPI_WP, active high */
	if (get_gpio(GPIO_SPI_WP))
		flags |= (1 << FLAG_SPI_WP);
	/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
	if (!get_gpio(GPIO_REC_MODE))
		flags |= (1 << FLAG_REC_MODE);

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
