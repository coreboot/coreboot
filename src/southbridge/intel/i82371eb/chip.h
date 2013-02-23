/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_I82371EB_CHIP_H
#define SOUTHBRIDGE_INTEL_I82371EB_CHIP_H

#include <device/device.h>

struct southbridge_intel_i82371eb_config {
	int ide0_enable:1;
	int ide0_drive0_udma33_enable:1;
	int ide0_drive1_udma33_enable:1;
	int ide1_enable:1;
	int ide1_drive0_udma33_enable:1;
	int ide1_drive1_udma33_enable:1;
	int ide_legacy_enable:1;
	int usb_enable:1;
	/* acpi */
	u32 gpo; /* gpio output default */
	u8 lid_polarity;
	u8 thrm_polarity;
};

#endif /* SOUTHBRIDGE_INTEL_I82371EB_CHIP_H */
