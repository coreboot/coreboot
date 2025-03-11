/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.LPCB.EC0)
{
	/* EC has data for GPU in memmap */
	Method (_QA0, 0, Serialized)
	{
		Local0 = ToInteger(GPUD)
		\_SB.PCI0.RP12.PXSX.DNOT (Local0, 0)
	}
}
