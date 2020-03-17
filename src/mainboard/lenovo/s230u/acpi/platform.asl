/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Method(_WAK,1)
{
	/* Turn on radios */
	Store (One, GP33) /* WLBT_OFF_5# (To pin 5 of WiFi mPCIe) */
	Store (One, GP36) /* WLBT_OFF_51# (To pin 51 of WiFi mPCIe) */
	/* There also is RF_OFF# on pin 20, controlled by the EC */

	Return(Package(){0,0})
}

Method(_PTS,1)
{
	/* Turn off radios */
	Store (Zero, GP33) /* WLBT_OFF_5# (To pin 5 of WiFi mPCIe) */
	Store (Zero, GP36) /* WLBT_OFF_51# (To pin 51 of WiFi mPCIe) */
	/* There also is RF_OFF# on pin 20, controlled by the EC */
}

Scope(\_SI)
{
	Method(_SST, 1, NotSerialized)
	{
		If (LLess(Arg0, 2))
		{
			/* Thinkpad LED on */
			\_SB.PCI0.LPCB.EC0.LED (Zero, 0x80)
		}
		Else
		{
			/* Thinkpad LED blinking */
			\_SB.PCI0.LPCB.EC0.LED (Zero, 0xC0)
		}
	}
}
