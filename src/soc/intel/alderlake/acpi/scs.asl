/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pcr_ids.h>

#define	PMCR_D0_MASK	0xFFFC
#define	PMCR_D3_MASK	0x0003

Scope (\_SB.PCI0) {

	/*
	 * Clear register 0x1C20/0x4820
	 * Arg0 - PCR Port ID
	 */
	Method(SCSC, 1, Serialized)
	{
		PCRA (Arg0, 0x1C20, 0x0)
		PCRA (Arg0, 0x4820, 0x0)
	}

	/* EMMC */
	Device(PEMC) {
		Name(_ADR, 0x001A0000)
		Name (_DDN, "eMMC Controller")
		Name(TEMP, 0)

		OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
		Field(SCSR, WordAcc, NoLock, Preserve) {
			Offset (0x84),	/* PMECTRLSTATUS */
			PMCR, 16,
			Offset (0xA2),	/* PG_CONFIG */
			, 2,
			PGEN, 1,	/* PG_ENABLE */
		}

		Method(_INI) {
			/*
			 * Clear eMMC timeout registers. _PS0 is not called by kernel when
			 * boot source is not eMMC, but OS still initializes eMMC. So disable
			 * timeout registers when boot source is not eMMC. Ported from CB:25290.
			 */
			SCSC (PID_EMMC)
		}

		Method(_PS0, 0, Serialized) {
			Stall (50)	/* Sleep 50 us */

			PGEN = 0	/* Disable PG */

			/* Clear register 0x1C20/0x4820 */
			SCSC (PID_EMMC)

			/* Set Power State to D0 */
			PMCR &= PMCR_D0_MASK
			/* Additional config read to eMMC controller. Ported from CB:23312 */
			TEMP = PMCR
		}

		Method(_PS3, 0, Serialized) {
			PGEN = 1	/* Enable PG */

			/* Set Power State to D3 */
			PMCR |= PMCR_D3_MASK
			/* Additional config read to eMMC controller. Ported from CB:23312 */
			TEMP = PMCR
		}

		Device (CARD)
		{
			Name (_ADR, 0x00000008)
			Method (_RMV, 0, NotSerialized)
			{
				Return (0)
			}
		}
	}
}
