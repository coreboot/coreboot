/* SPDX-License-Identifier: GPL-2.0-only */

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
	\_TZ.MEB1 = 0
	\_TZ.MEB2 = 0

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
		/* Determine the Operating System and save the value in OSYS.
		 * We have to do this in order to be able to work around
		 * certain windows bugs.
		 *
		 *    OSYS value | Operating System
		 *    -----------+------------------
		 *       2000    | Windows 2000
		 *       2001    | Windows XP(+SP1)
		 *       2002    | Windows XP SP2
		 *       2006    | Windows Vista
		 *       ????    | Windows 7
		 */

		/* Let's assume we're running at least Windows 2000 */
		OSYS =  2000

		If (CondRefOf(_OSI)) {
			If (_OSI("Windows 2001")) {
				OSYS = 2001
			}

			If (_OSI("Windows 2001 SP1")) {
				OSYS = 2001
			}

			If (_OSI("Windows 2001 SP2")) {
				OSYS = 2002
			}

			If (_OSI("Windows 2001.1")) {
				OSYS = 2001
			}

			If (_OSI("Windows 2001.1 SP1")) {
				OSYS = 2001
			}

			If (_OSI("Windows 2006")) {
				OSYS = 2006
			}

			If (_OSI("Windows 2006.1")) {
				OSYS = 2006
			}

			If (_OSI("Windows 2006 SP1")) {
				OSYS = 2006
			}

			If (_OSI("Windows 2009")) {
				OSYS = 2009
			}

			If (_OSI("Windows 2012")) {
				OSYS = 2012
			}
		}
	}
}
