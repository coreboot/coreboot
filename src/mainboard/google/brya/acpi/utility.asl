/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Poll a GPIO until it goes to the specified state
 * Arg0 == GPIO #
 * Arg1 == state (0 or 1)
 * Arg2 == timeout in ms
 */
Method (GPPL, 3, Serialized)
{
	Local0 = 0
	While (Local0 < Arg2)
	{
		If (\_SB.PCI0.GRXS (Arg0) == Arg1) {
			Return (0)
		} Else {
			Local0++
		}
		Sleep (1)
	}

	If (Local0 == Arg2) {
		Printf("[ERROR] GPPL for %o timed out", Arg0)
	}

	Return (0xFF)
}

/* Convert from 32-bit integer to 4-byte buffer (little-endian) */
Method (ITOB, 1)
{
	Local0 = Buffer(4) { 0, 0, 0, 0 }
	Local0[0] = Arg0 & 0xFF
	Local0[1] = (Arg0 >> 8) & 0xFF
	Local0[2] = (Arg0 >> 16) & 0xFF
	Local0[3] = (Arg0 >> 24) & 0xFF
	Return (Local0)
}
