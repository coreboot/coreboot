/* SPDX-License-Identifier: GPL-2.0-only */

/* Include in each PCIe Root Port device */

/* lowest D-state supported by
 * PCIe root port during S0 state
 */
Name (_S0W, 4)

Name (PDST, 0) /* present Detect status */

/* Dynamic Opregion needed to access registers
 * when the controller is in D3 cold
 */
OperationRegion (PX01, PCI_Config, 0x00, 0xFF)
Field (PX01, AnyAcc, NoLock, Preserve)
{
	Offset(0x5A),
	, 6,
	PDS, 1,		/* 6, Presence detect Change */
	Offset(0xE2),	/* RPPGEN - Root Port Power Gating Enable */
	, 2,
	L23E, 1,	/* 2, L23_Rdy Entry Request (L23ER) */
	L23R, 1,	/* 3, L23_Rdy to Detect Transition (L23R2DT) */
	Offset(0xF4),	/* BLKPLLEN */
	, 10,
	BPLL, 1,
}

OperationRegion (PX02, PCI_Config, 0x338, 0x4)
Field (PX02, AnyAcc, NoLock, Preserve)
{
	, 26,
	BDQA, 1		/* BLKDQDA */
}

PowerResource (PXP, 0, 0)
{
	/* Define the PowerResource for PCIe slot */
	Method (_STA, 0, Serialized)
	{
		PDST = PDS
		If (PDS == 1) {
			Return (0xf)
		} Else {
			Return (0)
		}
	}

	Method (_ON, 0, Serialized)
	{
		If (PDST == 1 && \PRT0 != 0) {
			/* Enter this condition if device
			 * is connected
			 */

			/* De-assert PERST */
			\_SB.PCI0.PRDA (\PRT0)

			BDQA  = 0 /* Set BLKDQDA to 0 */
			BPLL  = 0 /* Set BLKPLLEN to 0 */

			/* Set L23_Rdy to Detect Transition
			 * (L23R2DT)
			 */
			L23R = 1
			Sleep (16)
			Local0 = 0

			/* Delay for transition Detect
			 * and link to train
			 */
			While (L23R) {
				If (Local0 > 4) {
					Break
				}
				Sleep (16)
				Local0++
			}
		} /* End PDS condition check */
	}

	Method (_OFF, 0, Serialized)
	{
		/* Set L23_Rdy Entry Request (L23ER) */
		If (PDST == 1 && \PRT0 != 0) {
			/* enter this condition if device
			 * is connected
			 */
			L23E = 1
			Sleep (16)
			Local0 = 0
			While (L23E) {
				If (Local0 > 4) {
					Break
				}
				Sleep (16)
				Local0++
			}
			BDQA  = 1 /* Set BLKDQDA to 1 */
			BPLL  = 1 /* Set BLKPLLEN to 1 */

			/* Assert PERST */
			\_SB.PCI0.PRAS (\PRT0)
		} /* End PDS condition check */
	} /* End of Method_OFF */
} /* End PXP */

Name(_PR0, Package() { PXP })
Name(_PR3, Package() { PXP })
