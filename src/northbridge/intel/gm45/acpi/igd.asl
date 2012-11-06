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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

Device (GFX0)
{
	Name (_ADR, 0x00020000)

	/* Display Output Switching */
	Method (_DOS, 1)
	{
		/* Windows 2000 and Windows XP call _DOS to enable/disable
		 * Display Output Switching during init and while a switch
		 * is already active
		 */
		Store (And(Arg0, 7), DSEN)
	}

	/* We try to support as many GM45 systems as possible,
	 * so keep the number of DIDs flexible.
	 */
	Method (_DOD, 0)
	{
		If (LEqual(NDID, 1)) {
			Name(DOD1, Package() {
				0xffffffff
			})
			Store (Or(0x00010000, DID1), Index(DOD1, 0))
			Return(DOD1)
		}

		If (LEqual(NDID, 2)) {
			Name(DOD2, Package() {
				0xffffffff,
				0xffffffff
			})
			Store (Or(0x00010000, DID1), Index(DOD2, 0))
			Store (Or(0x00010000, DID2), Index(DOD2, 1))
			Return(DOD2)
		}

		If (LEqual(NDID, 3)) {
			Name(DOD3, Package() {
				0xffffffff,
				0xffffffff,
				0xffffffff
			})
			Store (Or(0x00010000, DID1), Index(DOD3, 0))
			Store (Or(0x00010000, DID2), Index(DOD3, 1))
			Store (Or(0x00010000, DID3), Index(DOD3, 2))
			Return(DOD3)
		}

		If (LEqual(NDID, 4)) {
			Name(DOD4, Package() {
				0xffffffff,
				0xffffffff,
				0xffffffff,
				0xffffffff
			})
			Store (Or(0x00010000, DID1), Index(DOD4, 0))
			Store (Or(0x00010000, DID2), Index(DOD4, 1))
			Store (Or(0x00010000, DID3), Index(DOD4, 2))
			Store (Or(0x00010000, DID4), Index(DOD4, 3))
			Return(DOD4)
		}

		If (LGreater(NDID, 4)) {
			Name(DOD5, Package() {
				0xffffffff,
				0xffffffff,
				0xffffffff,
				0xffffffff,
				0xffffffff
			})
			Store (Or(0x00010000, DID1), Index(DOD5, 0))
			Store (Or(0x00010000, DID2), Index(DOD5, 1))
			Store (Or(0x00010000, DID3), Index(DOD5, 2))
			Store (Or(0x00010000, DID4), Index(DOD5, 3))
			Store (Or(0x00010000, DID5), Index(DOD5, 4))
			Return(DOD5)
		}

		/* Some error happened, but we have to return something */
		Return (Package() {0x00000400})
	}

	Device(DD01)
	{
		/* Device Unique ID */
		Method(_ADR, 0, Serialized)
		{
			If(LEqual(DID1, 0)) {
				Return (1)
			} Else {
				Return (And(0xffff, DID1))
			}
		}

		/* Device Current Status */
		Method(_DCS, 0)
		{
			TRAP(1)
			If (And(CSTE, 1)) {
				Return (0x1f)
			}
			Return(0x1d)
		}

		/* Query Device Graphics State */
		Method(_DGS, 0)
		{
			If (And(NSTE, 1)) {
				Return(1)
			}
			Return(0)
		}

		/* Device Set State */
		Method(_DSS, 1)
		{
			/* If Parameter Arg0 is (1 << 31) | (1 << 30), the
			 * display switch was completed
			 */
			If (LEqual(And(Arg0, 0xc0000000), 0xc0000000)) {
				Store (NSTE, CSTE)
			}
		}
	}

	Device(DD02)
	{
		/* Device Unique ID */
		Method(_ADR, 0, Serialized)
		{
			If(LEqual(DID2, 0)) {
				Return (2)
			} Else {
				Return (And(0xffff, DID2))
			}
		}

		/* Device Current Status */
		Method(_DCS, 0)
		{
			TRAP(1)
			If (And(CSTE, 2)) {
				Return (0x1f)
			}
			Return(0x1d)
		}

		/* Query Device Graphics State */
		Method(_DGS, 0)
		{
			If (And(NSTE, 2)) {
				Return(1)
			}
			Return(0)
		}

		/* Device Set State */
		Method(_DSS, 1)
		{
			/* If Parameter Arg0 is (1 << 31) | (1 << 30), the
			 * display switch was completed
			 */
			If (LEqual(And(Arg0, 0xc0000000), 0xc0000000)) {
				Store (NSTE, CSTE)
			}
		}
	}


	Device(DD03)
	{
		/* Device Unique ID */
		Method(_ADR, 0, Serialized)
		{
			If(LEqual(DID3, 0)) {
				Return (3)
			} Else {
				Return (And(0xffff, DID3))
			}
		}

		/* Device Current Status */
		Method(_DCS, 0)
		{
			TRAP(1)
			If (And(CSTE, 4)) {
				Return (0x1f)
			}
			Return(0x1d)
		}

		/* Query Device Graphics State */
		Method(_DGS, 0)
		{
			If (And(NSTE, 4)) {
				Return(1)
			}
			Return(0)
		}

		/* Device Set State */
		Method(_DSS, 1)
		{
			/* If Parameter Arg0 is (1 << 31) | (1 << 30), the
			 * display switch was completed
			 */
			If (LEqual(And(Arg0, 0xc0000000), 0xc0000000)) {
				Store (NSTE, CSTE)
			}
		}
	}


	Device(DD04)
	{
		/* Device Unique ID */
		Method(_ADR, 0, Serialized)
		{
			If(LEqual(DID4, 0)) {
				Return (4)
			} Else {
				Return (And(0xffff, DID4))
			}
		}

		/* Device Current Status */
		Method(_DCS, 0)
		{
			TRAP(1)
			If (And(CSTE, 8)) {
				Return (0x1f)
			}
			Return(0x1d)
		}

		/* Query Device Graphics State */
		Method(_DGS, 0)
		{
			If (And(NSTE, 4)) {
				Return(1)
			}
			Return(0)
		}

		/* Device Set State */
		Method(_DSS, 1)
		{
			/* If Parameter Arg0 is (1 << 31) | (1 << 30), the
			 * display switch was completed
			 */
			If (LEqual(And(Arg0, 0xc0000000), 0xc0000000)) {
				Store (NSTE, CSTE)
			}
		}
	}


	Device(DD05)
	{
		/* Device Unique ID */
		Method(_ADR, 0, Serialized)
		{
			If(LEqual(DID5, 0)) {
				Return (5)
			} Else {
				Return (And(0xffff, DID5))
			}
		}

		/* Device Current Status */
		Method(_DCS, 0)
		{
			TRAP(1)
			If (And(CSTE, 16)) {
				Return (0x1f)
			}
			Return(0x1d)
		}

		/* Query Device Graphics State */
		Method(_DGS, 0)
		{
			If (And(NSTE, 4)) {
				Return(1)
			}
			Return(0)
		}

		/* Device Set State */
		Method(_DSS, 1)
		{
			/* If Parameter Arg0 is (1 << 31) | (1 << 30), the
			 * display switch was completed
			 */
			If (LEqual(And(Arg0, 0xc0000000), 0xc0000000)) {
				Store (NSTE, CSTE)
			}
		}
	}

}

