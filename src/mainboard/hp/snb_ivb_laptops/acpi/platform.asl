/* SPDX-License-Identifier: GPL-2.0-only */

Method(_WAK,1)
{
	\_SB.PCI0.LPCB.EC0.ACPI = 1
	\_SB.PCI0.LPCB.EC0.SLPT = 0

	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC0.SLPT = Arg0
}
