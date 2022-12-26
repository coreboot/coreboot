/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))

		Method (_LID)
		{
			if (\_SB.PCI0.LPCB.EC.LIDS > 1)
			{
				Return (1)
			}
			else
			{
				Return (Zero)
			}
		}

		Method (_STA)
		{
			Return (_LID)
		}
	}

}
