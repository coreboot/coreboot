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

Field(ERAM, ByteAcc, NoLock, Preserve)
{
                Offset (0x32),
			    , 2,
			WKLD, 1,
		Offset (0x46),
			    , 2,
			LIDS, 1
}

Device(LID)
{
	Name(_HID, "PNP0C0D")

	Method(_LId, 0, NotSerialized)
	{
		return (LIDS)
	}

	Method(_PRW, 0, NotSerialized)
	{
		Return (Package() { 0x18, 0x03 })
	}

	Method(_PSW, 1, NotSerialized)
	{
		if (Arg0) {
			Store(1, WKLD)
		} else {
			Store(0, WKLD)
		}
	}
}
