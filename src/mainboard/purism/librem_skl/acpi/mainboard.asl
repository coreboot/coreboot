/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))

		Method (_STA)
		{
			Return (0xF)
		}

		Method (_LID)
		{
			Return (\_SB.PCI0.LPCB.EC.LIDS)
		}
	}

	Device (SLPB)
	{
		Name (_HID, EisaId ("PNP0C0E"))

		Method (_STA)
		{
			Return (0xF)
		}
	}
}
