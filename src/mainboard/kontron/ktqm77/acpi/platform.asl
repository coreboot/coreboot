/* SPDX-License-Identifier: GPL-2.0-only */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	/* Let suspend LED flash slowly in S3 and S4 */
	If ((Arg0 == 3) || (Arg0 == 4))
	{
		\_SB.PCI0.LPCB.SIO0.SUSL (0x06)
	}
	Else
	{
		\_SB.PCI0.LPCB.SIO0.SUSL (0x02)
	}
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	/* Disable suspend LED during normal operation */
	\_SB.PCI0.LPCB.SIO0.SUSL (0x02)
	Return(Package(){0,0})
}
