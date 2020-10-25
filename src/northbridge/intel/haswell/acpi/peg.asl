/* SPDX-License-Identifier: GPL-2.0-only */

Field (\_SB.PCI0.MCHC.MCHP, DWordAcc, NoLock, Preserve)
{
	Offset (0x54),
	,	1,
	P2EN,	1,
	P1EN,	1,
	P0EN,	1,
}

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	Method (_STA)
	{
		Return (P0EN * 0xf)
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
		Return (P1EN * 0xf)
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
		Return (P2EN * 0xf)
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}
}
