/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include "smi.h"

Device (DSPC)
{
	Name (_ADR, 0x00020001)
	OperationRegion (DSPC, PCI_Config, 0x00, 0x100)
	Field (DSPC, ByteAcc, NoLock, Preserve)
	{
		Offset (0xf4),
		       BRTC, 8
	}

	Method(BRTD, 0, NotSerialized)
	{
		Trap(SMI_BRIGHTNESS_DOWN)
		Store(BRTC, Local0)
		if (LGreater (Local0, 15))
		{
			Subtract(Local0, 16, Local0)
			Store(Local0, BRTC)
		}
	}

	Method(BRTU, 0, NotSerialized)
	{
		Trap(SMI_BRIGHTNESS_UP)
		Store (BRTC, Local0)
		if (LLess(Local0, 0xff))
		{
			Add (Local0, 16, Local0)
			Store(Local0, BRTC)
		}
	}
}
