/* SPDX-License-Identifier: GPL-2.0-only */

Method(UCMS, 1, Serialized)
{
	Switch(ToInteger(Arg0))
	{
		Case (0x0c) /* Turn on ThinkLight */
		{
			\_SB.PCI0.LPCB.EC.LGHT(1)
		}
		Case (0x0d) /* Turn off ThinkLight */
		{
			\_SB.PCI0.LPCB.EC.LGHT(0)
		}
	}
}
