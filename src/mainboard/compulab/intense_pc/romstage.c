/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Hal Martin <hal.martin@gmail.com>
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
#include <lib.h>
#include <arch/io.h>
#include "northbridge/intel/sandybridge/raminit_native.h"

void pch_enable_lpc(void)
{
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x3f0f);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x0000164d);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x88, 0x000c0681);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x8c, 0x000406f1);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x90, 0x000c06a1);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0010);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xac, 0x00010000);
}

void rcba_config(void)
{
	/* Disable devices.  */
	RCBA32(0x3414) = 0x00000000;
	RCBA32(0x3418) = 0x16e81fe3;

}
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 1, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 1, 4 },
	{ 1, 1, 4 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
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
	read_spd(&spd[2], 0x52, id_only);
}
