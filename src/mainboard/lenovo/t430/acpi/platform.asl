/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Method(_WAK,1)
{
	/* ME may not be up yet. */
	Store (0, \_TZ.MEB1)
	Store (0, \_TZ.MEB2)

	/* Wake the HKEY to init BT/WWAN */
	\_SB.PCI0.LPCB.EC.HKEY.WAKE (Arg0)

	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC.MUTE(1)
	\_SB.PCI0.LPCB.EC.USBP(0)
	\_SB.PCI0.LPCB.EC.RADI(0)
}
