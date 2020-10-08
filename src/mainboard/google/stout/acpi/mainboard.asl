/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Name (_PRW, Package() {0x18, 4})

		Method (_LID, 0)
		{
			\LIDS = \_SB.PCI0.LPCB.EC0.HPLD
			Return (\LIDS)
		}

		Method (_PSW, 1)
		{
			// Enable/Disable LID as a wake source
			\_SB.PCI0.LPCB.EC0.HWLO = Arg0
		}
	}
}
