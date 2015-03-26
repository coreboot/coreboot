/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
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

Method (BRTN, 1, Serialized)
{
	If (LEqual (And (DID1, 0x0F00), 0x0400))
	{
		Notify (\_SB.PCI0.GFX0.DD01, Arg0)
	}

	If (LEqual (And (DID2, 0x0F00), 0x0400))
	{
		Notify (\_SB.PCI0.GFX0.DD02, Arg0)
	}

	If (LEqual (And (DID3, 0x0F00), 0x0400))
	{
		Notify (\_SB.PCI0.GFX0.DD03, Arg0)
	}

	If (LEqual (And (DID4, 0x0F00), 0x0400))
	{
		Notify (\_SB.PCI0.GFX0.DD04, Arg0)
	}

	If (LEqual (And (DID5, 0x0F00), 0x0400))
	{
		Notify (\_SB.PCI0.GFX0.DD05, Arg0)
	}
}
