/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2017 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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
	Field(GPIO, ByteAcc, NoLock, Preserve)
	{
		Offset(0x2c),	// GPIO Invert
		    ,   2,
		GV02,   1,
		    ,   1,
		GV04,   1,
	}

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

	/* Palm detect sensor 1 */
	Method (_L12, 0, NotSerialized) {
		// Invert trigger
		Store(GP02, GV02)

		PNOT (GP02, 0)
	}

	/* Palm detect sensor 2 */
	Method (_L14, 0, NotSerialized) {
		// Invert trigger
		Store(GP04, GV04)

		PNOT (GP04, 1)
	}
}
