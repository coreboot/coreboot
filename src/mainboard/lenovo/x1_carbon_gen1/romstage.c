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
#include <lib.h>
#include <timestamp.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cbfs.h>

void pch_enable_lpc(void)
{
	/* X230 EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, 0x0c06a1);

	pci_write_config32(PCH_LPC_DEV, ETR3, 0x10000);
}

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

void mainboard_rcba_config(void)
{
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	uint8_t *memory;
	const int spd_gpio_vector[] = {25, 45, -1};
	int spd_index = get_gpios(spd_gpio_vector);

	/* 4gb model = 0, 8gb model = 1 */
	/* int extended_memory_version = get_gpio(44); */
	/* TODO: how do they differ? Guess only one slot is connected */

	/*
	 * GPIO45 GPIO25
	 * 0      0       elpida
	 * 0      1       hynix
	 * 1      0       samsung
	 * 1      1       reserved
	 */

	/* We only support elpida and samsung.
           Because the spd data is missing. */
	if (spd_index != 0 && spd_index != 2)
		die("Unsupported Memory. Please add your SPD dump to coreboot.");

	memory = get_spd_data(spd_index);
	memcpy(&spd[0], memory, 256);
	memcpy(&spd[2], memory, 256);
}

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
}
