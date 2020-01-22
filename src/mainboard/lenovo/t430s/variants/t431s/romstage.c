/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <option.h>
#include <console/console.h>
#include <cbfs.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <string.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/lenovo/pmh7/pmh7.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },  /* SSP1: right */
	{ 1, 0, 1 },  /* SSP2: left, EHCI Debug */
	{ 1, 1, 3 },  /* SSP3: dock usb3 */
	{ 1, 1, -1 }, /* B0P4: wwan usb */
	{ 1, 1, 2 },  /* B0P5: dock usb2 */
	{ 0, 0, -1 }, /* B0P6 */
	{ 0, 0, -1 }, /* B0P7 */
	{ 1, 2, -1 }, /* B0P8: unknown */
	{ 1, 0, -1 }, /* B1P1: smart card reader */
	{ 0, 2, 5 },  /* B1P2 */
	{ 1, 1, -1 }, /* B1P3: fingerprint reader */
	{ 0, 0, -1 }, /* B1P4 */
	{ 1, 1, -1 }, /* B1P5: wlan usb */
	{ 1, 1, -1 }, /* B1P6: Camera */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	/* C1S0 is a soldered RAM with no real SPD. Use stored SPD. */
	size_t spd_file_len = 0;
	void *spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
		&spd_file_len);

	if (!spd_file || spd_file_len < sizeof(spd_raw_data))
		die("SPD data for C1S0 not found.");

	memcpy(&spd[0], spd_file, spd_file_len);
	read_spd(&spd[2], 0x51, id_only);
}
