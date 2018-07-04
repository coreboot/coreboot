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

#include <string.h>
#include <bootmode.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define GPIO_SPI_WP	24
#define GPIO_REC_MODE	42
#define GPIO_DEV_MODE	17

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1
#define FLAG_DEV_MODE	2

#ifndef __SIMPLE_DEVICE__
#include <boot/coreboot_tables.h>
#include "ec.h"
#include <ec/smsc/mec1308/ec.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	u16 gen_pmcon_1 = pci_read_config32(dev, GEN_PMCON_1);
	u8  lid = ec_read(0x83);

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO24 = KBC3_SPI_WP# */
	gpios->gpios[0].port = GPIO_SPI_WP;
	gpios->gpios[0].polarity = ACTIVE_HIGH;
	gpios->gpios[0].value = get_write_protect_state();
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: GPIO42 = CHP3_REC_MODE# */
	gpios->gpios[1].port = GPIO_REC_MODE;
	gpios->gpios[1].polarity = ACTIVE_LOW;
	gpios->gpios[1].value = !get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Developer: GPIO17 = KBC3_DVP_MODE */
	gpios->gpios[2].port = GPIO_DEV_MODE;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
	gpios->gpios[2].value = get_developer_mode_switch();
	strncpy((char *)gpios->gpios[2].name,"developer", GPIO_MAX_NAME_LENGTH);

	gpios->gpios[3].port = 100;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = lid & 1;
	strncpy((char *)gpios->gpios[3].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power Button */
	gpios->gpios[4].port = 101;
	gpios->gpios[4].polarity = ACTIVE_LOW;
	gpios->gpios[4].value = (gen_pmcon_1 >> 9) & 1;
	strncpy((char *)gpios->gpios[4].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Did we load the VGA Option ROM? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = gfx_get_init_done();
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);
}
#endif

int get_write_protect_state(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
	dev = PCI_DEV(0, 0x1f, 2);
#else
	struct device *dev;
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_SPI_WP) & 1;
}

int get_developer_mode_switch(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
	dev = PCI_DEV(0, 0x1f, 2);
#else
	struct device *dev;
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_DEV_MODE) & 1;
}

int get_recovery_mode_switch(void)
{
#ifdef __SIMPLE_DEVICE__
	pci_devfn_t dev;
	dev = PCI_DEV(0, 0x1f, 2);
#else
	struct device *dev;
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_REC_MODE) & 1;
}

void init_bootmode_straps(void)
{
#ifdef __PRE_RAM__
	u32 flags = 0;

	/* Write Protect: GPIO24 = KBC3_SPI_WP#, active high */
	if (get_gpio(GPIO_SPI_WP))
		flags |= (1 << FLAG_SPI_WP);
	/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
	if (!get_gpio(GPIO_REC_MODE))
		flags |= (1 << FLAG_REC_MODE);
	/* Developer: GPIO17 = KBC3_DVP_MODE, active high */
	if (get_gpio(GPIO_DEV_MODE))
		flags |= (1 << FLAG_DEV_MODE);

	pci_write_config32(PCI_DEV(0, 0x1f, 2), SATA_SP, flags);
#endif
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(GPIO_REC_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_DEV_AH(GPIO_DEV_MODE, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_SPI_WP, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
