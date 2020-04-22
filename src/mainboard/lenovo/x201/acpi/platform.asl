/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC.MUTE(1)
	\_SB.PCI0.LPCB.EC.USBP(0)
	\_SB.PCI0.LPCB.EC.RADI(0)
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	/* ME may not be up yet. */
	Store (0, \_TZ.MEB1)
	Store (0, \_TZ.MEB2)

	/* Wake the HKEY to init BT/WWAN */
	\_SB.PCI0.LPCB.EC.HKEY.WAKE (Arg0)

	/* Not implemented. */
	Return(Package(){0,0})
}

/* System Bus */

Scope(\_SB)
{
	/* This method is placed on the top level, so we can make sure it's the
	 * first executed _INI method.
	 */
	Method(_INI, 0)
	{
		/* The DTS data in NVS is probably not up to date.
		 * Update temperature values and make sure AP thermal
		 * interrupts can happen
		 */

		/* TRAP(71) */ /* TODO */

		\GOS()
	}
}
