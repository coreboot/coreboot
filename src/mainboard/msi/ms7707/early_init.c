/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_ops.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_pch_lpc_setup(void)
{
	u16 reg16;
	reg16 = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0xa4);
	reg16 |= (1 << 13); // WOL Enable Override (WOL_EN_OVRD)
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0xa4, reg16);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{1, 0, 0},
	{1, 0, 0},
	{1, 0, 1},
	{1, 0, 1},
	{1, 0, 2},
	{1, 0, 2},
	{1, 0, 3},
	{1, 0, 3},
	{1, 0, 4},
	{1, 0, 4},
	{1, 0, 6},
	{1, 0, 5},
	{1, 0, 5},
	{1, 0, 6},
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
