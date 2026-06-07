/* SPDX-License-Identifier: GPL-2.0-only */

/* Intel Gigabit Ethernet Controller 0:1f.6 */
#include <soc/gpe.h>

Device (GLAN)
{
	Name (_ADR, 0x001f0006)

	OperationRegion (GLCS, PCI_Config, 0x00, 0x04)
	Field (GLCS, DWordAcc, NoLock, Preserve)
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

	Name (_S0W, 3)

	Name (_PRW, Package() {GPE0_PME_B0, 4})

	Method (_DSW, 3) {}
}
