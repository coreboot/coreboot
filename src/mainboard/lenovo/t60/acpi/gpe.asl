/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_GPE)
{
	Method(_L18, 0, NotSerialized)
	{
		/* Read EC register to clear wake status */
		Local0 = \_SB.PCI0.LPCB.EC.WAKE
		/* So that we don't get a warning that Local0 is unused. */
		Local0++
	}
}
