/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_TZ)
{
	Name (MEBT, 0)

	Method(C2K, 1, NotSerialized)
	{
		Local0 = Arg0 * 10
		Local0 += 2732
		if (Local0 <= 2732) {
			Return (3000)
		}

		if (Local0 > 4012) {
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
			If (!MEBT && (\_SB.PCI0.LPCB.EC.TMP0 == 128)) {
				Return (C2K(40))
			}
			MEBT = 1
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
