/* SPDX-License-Identifier: GPL-2.0-only */

Scope (S0B0) {
	OperationRegion (NAPC, PCI_Config, 0xB4, 0x0C)
	Field (NAPC, DWordAcc, NoLock, Preserve)
	{
		NAEX,   32,
		NAPX,   32,
		NAPD,   32
	}
}

/*
 * Clears IoapicSbFeatureEn on every GNB IOAPIC to switch routing to IOAPIC.
 */
Mutex (NAPM, 0x00)
Method (NAPE, 0, NotSerialized)
{
	If (PICM == 0)
	{
		Return
	}

	Acquire (NAPM, 0xFFFF)
	/* Get number of root bridges */
	\_SB.S0B0.NAEX = 0
	\_SB.S0B0.NAPX = 0x49004184
	Local0 = \_SB.S0B0.NAPD
	Local0 >>= 0x08
	Local0 &= 0xFF
	/* Get number of sockets present */
	\_SB.S0B0.NAEX = 0
	\_SB.S0B0.NAPX = 0x49004180
	Local3 = \_SB.S0B0.NAPD
	Local3 >>= 0x08
	Local3 &= One
	If (Local3)
	{
		Local3 = 0x02
	}
	Else
	{
		Local3 = 1
	}

	Local0 /= Local3
	\_SB.S0B0.NAEX = 0
	While ((Local3 > 0))
	{
		Local4 = Local0
		While ((Local4 > 0))
		{
			If ((Local4 > 0x04))
			{
				Local5 = 0x1D800000
				Local1 = (Local4 - 0x05)
			}
			Else
			{
				Local5 = 0x14300000
				Local1 = (Local4 - 1)
			}

			Local1 *= 0x00100000
			Local1 += Local5
			\_SB.S0B0.NAPX = Local1
			Local2 = \_SB.S0B0.NAPD
			Local2 &= 0xFFFFFFEF
			\_SB.S0B0.NAPD = Local2
			Local4--
		}

		Local3--
		\_SB.S0B0.NAEX = 1
	}

	\_SB.S0B0.NAEX = 0
	Release (NAPM)
}
