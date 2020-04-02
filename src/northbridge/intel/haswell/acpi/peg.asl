/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	Method (_STA)
	{
		Return (((\_SB.PCI0.MCHC.DVEN >> 3) & 1) * 0xf)
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}

Device (PEG1)
{
	Name (_ADR, 0x00010001)

	Method (_STA)
	{
		Return (((\_SB.PCI0.MCHC.DVEN >> 2) & 1) * 0xf)
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}

Device (PEG2)
{
	Name (_ADR, 0x00010002)

	Method (_STA)
	{
		Return (((\_SB.PCI0.MCHC.DVEN >> 1) & 1) * 0xf)
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}
