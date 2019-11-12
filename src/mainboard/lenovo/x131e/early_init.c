/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2017 James Ye <jye836@gmail.com>
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

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{1, 1, 0},	/* P0: USB 3.0 1 (OC0) */
	{1, 1, 0},	/* P1: USB 3.0 2 (OC0) */
	{0, 0, 0},
	{1, 1, -1},	/* P3: Camera (no OC) */
	{1, 0, -1},	/* P4: WLAN (no OC) */
	{1, 0, -1},	/* P5: WWAN (no OC) */
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{1, 1, 4},	/* P9: USB 2.0 (AUO4) (OC4) */
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{1, 0, -1},	/* P13: Bluetooth (no OC) */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
