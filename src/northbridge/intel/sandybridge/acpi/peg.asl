/* SPDX-License-Identifier: GPL-2.0-only */

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

	Method (_PRT)
	{
		Return (\_SB.PCI0.IRQM (1))
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

	Method (_PRT)
	{
		Return (\_SB.PCI0.IRQM (2))
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

	Method (_PRT)
	{
		Return (\_SB.PCI0.IRQM (3))
	}
}

Device (PEG6)
{
	Name (_ADR, 0x00060000)

	Method (_STA)
	{
		Return (((\_SB.PCI0.MCHC.DVEN >> 13) & 1) * 0xf)
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
	}

	Method (_PRT)
	{
		Return (\_SB.PCI0.IRQM (4))
	}
}
