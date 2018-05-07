/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2016 Intel Corporation
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
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/nhlt.h>

#define SUBSYSTEM_ID 0x006B

static const char *oem_id_maxim = "GOOGLE";
static const char *oem_table_id_maxim = "EVEMAX";

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;

	start_addr = current;

	nhlt = nhlt_init();
	if (!nhlt)
		return start_addr;

	nhlt->subsystem_id = SUBSYSTEM_ID;

	/* 4 Channel DMIC array */
	if (nhlt_soc_add_rt5514(nhlt, AUDIO_LINK_SSP0, 4))
		printk(BIOS_ERR, "Couldn't add rt5514.\n");

	/* RT5663 Headset codec */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add headset codec.\n");

	/* MAXIM98927 Smart Amps for left and right channel */
	if (nhlt_soc_add_max98927(nhlt, AUDIO_LINK_SSP0))
		printk(BIOS_ERR, "Couldn't add max98927\n");

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
				    oem_id_maxim, oem_table_id_maxim, 0);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
