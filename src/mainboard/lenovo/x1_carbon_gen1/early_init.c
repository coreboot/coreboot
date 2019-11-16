/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Alexander Couzens <lynxis@fe80.eu>
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
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <cbfs.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled, current, OC pin */
	{ 0, 3, 0 }, /* P00 disconnected */
	{ 1, 1, 1 }, /* P01 left or right */
	{ 0, 1, 3 }, /* P02 disconnected */
	{ 1, 3, -1 },/* P03 WWAN */
	{ 0, 1, 2 }, /* P04 disconnected */
	{ 0, 1, -1 },/* P05 disconnected */
	{ 0, 1, -1 },/* P06 disconnected */
	{ 0, 2, -1 },/* P07 disconnected */
	{ 0, 1, -1 },/* P08 disconnected */
	{ 1, 2, 5 }, /* P09 left or right */
	{ 1, 3, -1 },/* P10 FPR */
	{ 1, 3, -1 },/* P11 Bluetooth */
	{ 1, 1, -1 },/* P12 WLAN */
	{ 1, 1, -1 },/* P13 Camera */
};

static uint8_t *get_spd_data(int spd_index)
{
	uint8_t *spd_file;
	size_t spd_file_len;

	printk(BIOS_DEBUG, "spd index %d\n", spd_index);
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
						&spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < spd_index * 256)
		die("Missing SPD data.");

	return spd_file + spd_index * 256;
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	uint8_t *memory;
	const int spd_gpio_vector[] = {25, 45, -1};
	int spd_index = get_gpios(spd_gpio_vector);

	/* 4gb model = 0, 8gb model = 1 */
	/* int extended_memory_version = get_gpio(44); */

	/*
	 * So far there is no need to parse gpio 44, as the 4GiB use
	 * the hynix or elpida memory and 8 GiB versions use samsung.
	 * All version use both channels.
	 * But we might miss some versions.
	 */

	/*
	 * GPIO45 GPIO25
	 * 0      0       elpida
	 * 0      1       hynix
	 * 1      0       samsung
	 * 1      1       reserved
	 */

	if (spd_index == 3)
		die("Unsupported Memory. (detected 'reserved' memory configuration).");

	memory = get_spd_data(spd_index);
	memcpy(&spd[0], memory, 256);
	memcpy(&spd[2], memory, 256);
}
