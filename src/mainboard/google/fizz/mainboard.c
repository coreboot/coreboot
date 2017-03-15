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
#include <soc/nhlt.h>
#include <vendorcode/google/chromeos/chromeos.h>

static const char *oem_id_maxim = "GOOGLE";
static const char *oem_table_id_maxim = "FIZZMAX";

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

	start_addr = current;

	nhlt = nhlt_init();

	if (nhlt == NULL)
		return start_addr;

	/* 2 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 2))
		printk(BIOS_ERR, "Couldn't add 2CH DMIC array.\n");

	/* 4 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 4))
		printk(BIOS_ERR, "Couldn't add 4CH DMIC arrays.\n");

	/* Maxim MAX98927 Smart Amps for left and right channel */
	if (nhlt_soc_add_max98927(nhlt, AUDIO_LINK_SSP0))
		printk(BIOS_ERR, "Couldn't add Maxim MAX98927\n");

	/* Realtek RT5663 Headset codec. */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add Realtek RT5663.\n");

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
			oem_id_maxim, oem_table_id_maxim, 0);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
