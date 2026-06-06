/* SPDX-License-Identifier: GPL-2.0-only */

/* GNA Scoring Accelerator - Device 08, Function 0 */
Device (GNA)
{
	Name (_ADR, 0x00080000)
	Name (_DDN, "GNA Scoring Accelerator")

	OperationRegion (GNAC, PCI_Config, 0x00, 0x04)
	Field (GNAC, DWordAcc, NoLock, Preserve)
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
