/* SPDX-License-Identifier: GPL-2.0-only */

Method(_WAK,1)
{
	/* Turn on radios */
	GP33 = 1 /* WLBT_OFF_5# (To pin 5 of WiFi mPCIe) */
	GP36 = 1 /* WLBT_OFF_51# (To pin 51 of WiFi mPCIe) */
	/* There also is RF_OFF# on pin 20, controlled by the EC */

	Return(Package(){0,0})
}

Method(_PTS,1)
{
	/* Turn off radios */
	GP33 = 0 /* WLBT_OFF_5# (To pin 5 of WiFi mPCIe) */
	GP36 = 0 /* WLBT_OFF_51# (To pin 51 of WiFi mPCIe) */
	/* There also is RF_OFF# on pin 20, controlled by the EC */
}

Scope(\_SI)
{
	Method(_SST, 1, NotSerialized)
	{
		If (Arg0 < 2)
		{
			/* Thinkpad LED on */
			\_SB.PCI0.LPCB.EC0.LED (0, 0x80)
		}
		Else
		{
			/* Thinkpad LED blinking */
			\_SB.PCI0.LPCB.EC0.LED (0, 0xC0)
		}
	}
}
