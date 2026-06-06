/* SPDX-License-Identifier: GPL-2.0-only */

/* CSE/HECI interface - Device 16, Function 0 */
Device (HECI)
{
	Name (_ADR, 0x00160000)

	OperationRegion (HECS, PCI_Config, 0x00, 0x04)
	Field (HECS, DWordAcc, NoLock, Preserve)
	{
		VDID, 32,
	}

	Method (_STA, 0, NotSerialized)
	{
		If (VDID != 0xFFFFFFFF) {
			Return (0x0F)
		}

		Return (0)
	}
}
