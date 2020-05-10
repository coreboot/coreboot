/* SPDX-License-Identifier: GPL-2.0-or-later */

Method(_WAK,1)
{
	/* ME may not be up yet. */
	Store (0, \_TZ.MEB1)
	Store (0, \_TZ.MEB2)
	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC.RADI(0)
}
