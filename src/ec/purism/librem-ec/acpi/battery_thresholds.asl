/* SPDX-License-Identifier: GPL-2.0-only */

Field (ERAM, ByteAcc, Lock, Preserve)
{
	Offset (0xBC),
	BTL0,   8,      /* BAT0 charging start threshold */
	BTH0,   8,      /* BAT0 charging end threshold */
}

/*
 * Get battery charging threshold
 *
 * Arg0: 0: Start threshold
 *       1: Stop threshold
 */
Method (GBCT, 1, NotSerialized)
{
	If (Arg0 == 0) {
		Return (BTL0)
	}

	If (Arg0 == 1) {
		Return (BTH0)
	}

	Return (0xFF)
}

/*
 * Set battery charging threshold
 *
 * Arg0: 0: Start threshold
 *       1: Stop threshold
 * Arg1: Percentage
 */
Method (SBCT, 2, NotSerialized)
{
	If (Arg1 <= 100) {
		If (Arg0 == 0) {
			BTL0 = Arg1
		}
		If (Arg0 == 1) {
			BTH0 = Arg1
		}
	}
}
