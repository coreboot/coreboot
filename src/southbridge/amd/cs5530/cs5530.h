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

#ifndef SOUTHBRIDGE_AMD_CS5530_CS5530_H
#define SOUTHBRIDGE_AMD_CS5530_CS5530_H

#if !defined(__PRE_RAM__)
#include "chip.h"
void cs5530_enable(device_t dev);
#endif

#define DECODE_CONTROL_REG2		0x5b		/* F0 index 0x5b */
#define ROM_AT_LOGIC_CONTROL_REG	0x52		/* F0 index 0x52 */

#define LOWER_ROM_ADDRESS_RANGE		(1 << 0)
#define ROM_WRITE_ENABLE		(1 << 1)
#define UPPER_ROM_ADDRESS_RANGE		(1 << 2)
#define BIOS_ROM_POSITIVE_DECODE	(1 << 5)

/* Selects PCI positive decoding for accesses to the configured ROM space. */
#define BIOS_ROM_POSITIVE_DECODE	(1 << 5)

/* Primary IDE Controller Positive Decode (i.e., enable it). */
#define PRIMARY_IDE_ENABLE		(1 << 3)

/* Secondary IDE Controller Positive Decode (i.e., enable it). */
#define SECONDARY_IDE_ENABLE		(1 << 4)

#endif				/* SOUTHBRIDGE_AMD_CS5530_CS5530_H */
