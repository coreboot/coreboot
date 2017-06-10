/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/pci_devs.h>
#include <soc/nhlt.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void mainboard_init(device_t dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(device_t device,
	unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;
	const char *oem_id = NULL;
	const char *oem_table_id = NULL;
	uint32_t oem_revision = 0;

	start_addr = current;

	nhlt = nhlt_init();

	if (nhlt == NULL)
		return start_addr;

	variant_nhlt_init(nhlt);
	variant_nhlt_oem_overrides(&oem_id, &oem_table_id, &oem_revision);

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
			oem_id, oem_table_id, oem_revision);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(device_t dev)
{
	device_t tpm;

	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;

	/* Disable unused interface(s) for TPM. */
	if (!IS_ENABLED(CONFIG_POPPY_USE_SPI_TPM)) {
		tpm = PCH_DEV_GSPI0;
		if (tpm)
			tpm->enabled = 0;
	}

	if (!IS_ENABLED(CONFIG_POPPY_USE_I2C_TPM)) {
		tpm = PCH_DEV_I2C1;
		if (tpm)
			tpm->enabled = 0;
	}
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
