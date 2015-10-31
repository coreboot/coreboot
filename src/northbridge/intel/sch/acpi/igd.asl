/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

Device (GFX0)
{
	Name (_ADR, 0x00020000)

	OperationRegion (GFXC, PCI_Config, 0x00, 0x0100)
	Field (GFXC, DWordAcc, NoLock, Preserve)
	{
		Offset (0x10),
		BAR0, 64
	}

	OperationRegion (GFRG, SystemMemory, And(BAR0, 0xfffffffffffffff0), 0x400000)
	Field (GFRG, DWordAcc, NoLock, Preserve)
	{
		Offset (0x61250),
			CR1, 32,
			BCLV, 16,
			BCLM, 16,
	}

	Name (BRIG, Package (0x12)
	{
		0x61,
		0x61,
		0x2,
		0x4,
		0x5,
		0x7,
		0x9,
		0xb,
		0xd,
		0x11,
		0x14,
		0x17,
		0x1c,
		0x20,
		0x27,
		0x31,
		0x41,
		0x61,
	})

	Method (XBCM, 1, NotSerialized)
	{
		Store (ShiftLeft (Arg0, 4), BCLV)
		Store (0x80000000, CR1)
		Store (0x0610, BCLM)
	}

	Method (XBQC, 0, NotSerialized)
	{
		Store (BCLV, Local0)
		ShiftRight (Local0, 4, Local0)
		Return (Local0)
	}
#include <drivers/intel/gma/acpi/common.asl>
}
