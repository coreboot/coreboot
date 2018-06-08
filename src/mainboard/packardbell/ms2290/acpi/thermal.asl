/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Scope(\_TZ)
{
	Name (MEBT, 0)

	Method(C2K, 1, NotSerialized)
	{
		Multiply(Arg0, 10, Local0)
		Add (Local0, 2732, Local0)
		if (LLessEqual(Local0, 2732)) {
			Return (3000)
		}

		if (LGreater(Local0, 4012)) {
			Return (3000)
		}
		Return (Local0)
	}

	ThermalZone(THM0)
	{
		Method(_CRT, 0, NotSerialized) {
			Return (C2K(127))
		}
		Method(_TMP) {
			/* Avoid tripping alarm if ME isn't booted at all yet */
			If (LAnd (LNot (MEBT), LEqual (\_SB.PCI0.LPCB.EC.TMP0, 128))) {
				Return (C2K(40))
			}
			Store (1, MEBT)
			Return (C2K(\_SB.PCI0.LPCB.EC.TMP0))
		}
	}

	ThermalZone(THM1)
	{
		Method(_CRT, 0, NotSerialized) {
			Return (C2K(99))
		}

		Method(_PSV, 0, NotSerialized) {
			Return (C2K(94))
		}

		Method(_TMP) {
			Return (C2K(\_SB.PCI0.LPCB.EC.TMP1))
		}
	}
}
