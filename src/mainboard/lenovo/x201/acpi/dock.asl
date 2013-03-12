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

Scope (\_SB)
{
	OperationRegion (DLPC, SystemIO, 0x164c, 1)
	Field(DLPC, ByteAcc, NoLock, Preserve)
	{
		    ,	3,
		DSTA,	1,
	}

	Device(DOCK)
	{
		Name(_HID, "ACPI0003")
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB } )

		Method(_DCK, 1, NotSerialized)
		{
			if (Arg0) {
			   /* connect dock */
			   TRAP(SMI_DOCK_CONNECT)
			} else {
			   /* disconnect dock */
			   TRAP(SMI_DOCK_DISCONNECT)
			}

			Xor(Arg0, DSTA, Local0)
			Return (Local0)
		}

		Method(_STA, 0, NotSerialized)
		{
			Return (DSTA)
		}
	}
}

Scope(\_SB.PCI0.LPCB.EC)
{
	Method(_Q18, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 3)
	}

	Method(_Q50, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 3)
	}

	Method(_Q58, 0, NotSerialized)
	{
	       Notify(\_SB.DOCK, 0)
	}

}
