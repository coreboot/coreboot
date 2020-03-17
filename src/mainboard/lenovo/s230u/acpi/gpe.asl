/*
 * This file is part of the coreboot project.
 *
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

Scope (_GPE)
{
	Name (PDET, Zero)
	Method (PNOT, 2, Serialized) {
		ShiftLeft (Arg0, Arg1, Local0)
		Not( ShiftLeft (One, Arg1), Local1)
		Or (Local0, And (Local1, PDET), PDET)
		If (LEqual (PDET, Zero)) {
			// Palm removed
			\_SB.PCI0.LPCB.EC0.HKEY.MHKQ (0x60B1)
		} Else {
			// Palm detected
			\_SB.PCI0.LPCB.EC0.HKEY.MHKQ (0x60B0)
		}
	}

	Method (TINV, 2, Serialized) {
		ShiftLeft (One, Arg1, Local0)
		If (LEqual (Arg0, Zero)) {
			Not (Local0, Local0)
			And (GIV0, Local0, GIV0)
		} Else {
			Or (GIV0, Local0, GIV0)
		}
	}

	/* Palm detect sensor 1 */
	Method (_L12, 0, NotSerialized) {
		// Invert trigger
		TINV (GP02, 2)

		PNOT (GP02, 0)
	}

	/* Palm detect sensor 2 */
	Method (_L14, 0, NotSerialized) {
		// Invert trigger
		TINV (GP04, 4)

		PNOT (GP04, 1)
	}
}
