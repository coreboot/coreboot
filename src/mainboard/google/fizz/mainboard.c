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
#include <soc/nhlt.h>
#include <vendorcode/google/chromeos/chromeos.h>

static const char *oem_id = "GOOGLE";
static const char *oem_table_id = "FIZZ";

static void mainboard_init(device_t dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	device_t device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;

	start_addr = current;

	nhlt = nhlt_init();
	if (!nhlt)
		return start_addr;

	/* RT5663 Headset codec */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add headset codec.\n");

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
				oem_id, oem_table_id, 0);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
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

	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
