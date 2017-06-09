/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
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

#include <arch/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/pci_devs.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void mainboard_init(device_t dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(device_t dev)
{
	device_t tpm;

	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;

	/* Disable unused interface for TPM. */
	if (!IS_ENABLED(CONFIG_FIZZ_USE_SPI_TPM)) {
		tpm = PCH_DEV_GSPI0;
		if (tpm)
			tpm->enabled = 0;
	}

	if (!IS_ENABLED(CONFIG_FIZZ_USE_I2C_TPM)) {
		tpm = PCH_DEV_I2C1;
		if (tpm)
			tpm->enabled = 0;
	}
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
