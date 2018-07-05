/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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
#include <string.h>
#include <cbfs.h>
#include <lib.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include "ec.h"

#define SPD_LEN 256

void pch_enable_lpc(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x0c00);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x00000000);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x88, 0x000c0701);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x8c, 0x000c0069);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x90, 0x000c06a1);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), ETR3, 0x10000);

	/* Memory map KB9012 EC registers */
	pci_write_config32(
		PCI_DEV(0, 0x1f, 0), 0x98,
		CONFIG_EC_BASE_ADDRESS | 1);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0xd8, 0xffc0);

	/* Enable external USB port power. */
	if (IS_ENABLED(CONFIG_USBDEBUG))
		ec_mm_set_bit(0x3b, 4);
}

void mainboard_rcba_config(void)
{
	/* Disable devices.  */
	RCBA32(0x3414) = 0x00000020;
}
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 0, 0 },
	{ 1, 1, 1 },
	{ 1, 0, 1 },
	{ 1, 1, 2 },
	{ 1, 0, 2 },
	{ 0, 0, 3 },
	{ 0, 1, 3 },
	{ 1, 0, 4 },
	{ 1, 1, 4 },
	{ 1, 1, 5 },
	{ 1, 1, 5 },
	{ 1, 1, 6 },
	{ 1, 1, 6 },
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
}

static const char *mainboard_spd_names[9] = {
	"ELPIDA 4GB",
	"SAMSUNG 4GB",
	"HYNIX 4GB",
	"ELPIDA 8GB",
	"SAMSUNG 8GB",
	"HYNIX 8GB",
	"ELPIDA 2GB",
	"SAMSUNG 2GB",
	"HYNIX 2GB",
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	void *spd_file;
	size_t spd_file_len = 0;
	const int spd_gpios[] = {71, 70, 16, 48, -1};

	u32 spd_index = get_gpios(spd_gpios);
	if (spd_index >= ARRAY_SIZE(mainboard_spd_names)) {
		/* Fallback to pessimistic 2GB image (ELPIDA 2GB) */
		spd_index = 6;
	}

	printk(BIOS_INFO, "SPD index %d (%s)\n",
		spd_index, mainboard_spd_names[spd_index]);

	/* C0S0 is a soldered RAM with no real SPD. Use stored SPD.  */
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
		&spd_file_len);

	if (!spd_file || spd_file_len < SPD_LEN * spd_index + SPD_LEN)
		die("SPD data not found.");

	memcpy(spd, spd_file + SPD_LEN * spd_index, SPD_LEN);
}
