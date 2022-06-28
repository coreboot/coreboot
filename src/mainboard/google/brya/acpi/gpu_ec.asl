/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_D_NOTIFY_MASK	0x7

Scope (\_SB.PCI0.LPCB.EC0)
{
	/* EC has data for GPU in memmap */
	Method (_Q0C, 0, Serialized)
	{
		Local0 = ToInteger(GPUD) & EC_D_NOTIFY_MASK
		\_SB.PCI0.PEG0.PEGP.DNOT (Local0)
	}
}
