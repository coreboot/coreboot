/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_GPE)
{
	Method(_L18, 0, NotSerialized)
	{
		/* Read EC register to clear wake status */
		Store(\_SB.PCI0.LPCB.EC.WAKE, Local0)
		/* So that we don't get a warning that Local0 is unused. */
		Increment (Local0)
	}
}
