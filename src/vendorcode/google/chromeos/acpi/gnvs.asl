/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/* This is the ChromeOS specific ACPI information needed by
 * the mainboard's chromeos.asl
 */

VBT0,   32,	// 0x000 - Boot Reason
VBT1,   32,	// 0x004 - Active Main Firmware
VBT2,   32,	// 0x008 - Active EC Firmware
VBT3,   16,	// 0x00c - CHSW
VBT4, 2048,	// 0x00e - HWID
VBT5,  512,	// 0x10e - FWID
VBT6,  512,	// 0x14e - FRID
VBT7,   32,	// 0x18e - active main firmware type
VBT8,   32,	// 0x192 - Recovery Reason
VBT9,   32,	// 0x196 - FMAP base address
CHVD, 24576,	// 0x19a - VDAT space filled by verified boot
VBTA,	32,	// 0xd9a - pointer to smbios FWID
MEHH,  256,	// 0xd9e - Management Engine Hash
		// 0xdbe
