/* SPDX-License-Identifier: GPL-2.0-or-later */

Method(_WAK,1)
{
	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC.RADI(0)
}
