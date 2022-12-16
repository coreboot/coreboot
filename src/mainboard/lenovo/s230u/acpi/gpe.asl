/* SPDX-License-Identifier: GPL-2.0-only */

Scope (_GPE)
{
	Name (PDET, Zero)
	Method (PNOT, 2, Serialized) {
		Local0 = Arg0 << Arg1
		Not(One << Arg1, Local1)
		Or (Local0, And (Local1, PDET), PDET)
		If (PDET == Zero) {
			// Palm removed
			\_SB.PCI0.LPCB.EC0.HKEY.MHKQ (0x60B1)
		} Else {
			// Palm detected
			\_SB.PCI0.LPCB.EC0.HKEY.MHKQ (0x60B0)
		}
	}

	Method (TINV, 2, Serialized) {
		Local0 = One << Arg1
		If (Arg0 == Zero) {
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
