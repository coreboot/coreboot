/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))

		Method (_LID)
		{
			if (\_SB.PCI0.LPCB.EC.LIDS > 1)
			{
				Return (One)
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

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))

		Method (_STA)
		{
			Return (0xF)
		}

		Name (_PRW, Package () { 27, 4 })
	}

}
