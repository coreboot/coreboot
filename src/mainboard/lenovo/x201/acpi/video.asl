/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (c) 2013 Vladimir Serbinenko
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

Scope (\_SB.PCI0.GFX0)
{
	Device (LCD0)
	{
		Name (_ADR, 0x0400)
		Name (BRCT, 0)

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

		Method (_BCL, 0, NotSerialized)
		{
			Store (1, BRCT)
			Return (BRIG)
		}

		Method (_BCM, 1, NotSerialized)
		{
			Store (ShiftLeft (Arg0, 4), ^^BCLV)
			Store (0x80000000, ^^CR1)
			Store (0x061a061a, ^^CR2)
		}
		Method (_BQC, 0, NotSerialized)
		{
			Store (^^BCLV, Local0)
			ShiftRight (Local0, 4, Local0)
			Return (Local0)
		}

		Method(BRID, 1, NotSerialized)
		{
			Store (Match (BRIG, MEQ, Arg0, MTR, Zero, 2), Local0)
			If (LEqual (Local0, Ones))
			{
				Return (0x11)
			}
			Return (Local0)
		}

		/* Using Notify is the right way. But Windows doesn't handle
		   it well. So use both method in a way to avoid double action.
		 */
		Method (DECB, 0, NotSerialized)
		{
			If (BRCT)
			{
				Notify (LCD0, 0x87)
			} Else {
				Store (BRID (_BQC ()), Local0)
				If (LNotEqual (Local0, 2))
				{
					Decrement (Local0)
				}
				_BCM (DerefOf (Index (BRIG, Local0)))
			}
		}
		Method (INCB, 0, NotSerialized)
		{
			If (BRCT)
			{
				Notify (LCD0, 0x86)
			} Else {
				Store (BRID (_BQC ()), Local0)
				If (LNotEqual (Local0, 0x11))
				{
					Increment (Local0)
				}
				_BCM (DerefOf (Index (BRIG, Local0)))
			}
		}
	}
}
