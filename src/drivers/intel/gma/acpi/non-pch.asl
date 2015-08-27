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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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

	OperationRegion (GFRG, SystemMemory, And (BAR0, 0xfffffffffffffff0), 0x400000)
	Field (GFRG, DWordAcc, NoLock, Preserve)
	{
		Offset (0x61254),
		BCLV, 16,
		BCLM, 16,
	}

#include "configure_brightness_levels.asl"
#include "common.asl"
}
