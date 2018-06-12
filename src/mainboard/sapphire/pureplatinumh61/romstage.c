/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Nicola Corna <nicola@corna.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#include <arch/acpi.h>
#include <console/console.h>
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit_native.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <delay.h>

void pch_enable_lpc(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x2400);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x000c0291);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x88, 0x000c0a01);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x8c, 0x00000000);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x90, 0x00000000);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0000);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xac, 0x00010000);
}

void mainboard_rcba_config(void)
{
	/* Disable devices.  */
	RCBA32(0x3414) = 0x00000020;
}
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 1 },
	{ 1, 0, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 4 },
	{ 1, 0, 4 },
	{ 1, 0, 6 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}
