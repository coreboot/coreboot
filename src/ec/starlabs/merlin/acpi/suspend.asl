/* SPDX-License-Identifier: GPL-2.0-only */

Method (RPTS, 1, Serialized)
{
	If ((Arg0 == 0x04) || (Arg0 == 0x05))
	{
		/* Store current EC settings in CMOS */
		Switch (ToInteger (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.TPLE))))
		{
			// 0x00 == Enabled    == 0x00
			// 0x11 == Re-enabled == 0x00
			// 0x22 == Disabled   == 0x01
			Case (0x00)
			{
				\_SB.PCI0.LPCB.TPLC = 0x00
			}
			Case (0x11)
			{
				\_SB.PCI0.LPCB.TPLC = 0x00
			}
			Case (0x22)
			{
				\_SB.PCI0.LPCB.TPLC = 0x01
			}
		}

		\_SB.PCI0.LPCB.FLKC =
			\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.FLKE))

		Switch (ToInteger (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLSE))))
		{
			// 0x00 == Disabled == 0x00
			// 0xdd == Enabled  == 0x01
			Case (0x00)
			{
				\_SB.PCI0.LPCB.KLSC = 0x00
			}
			Case (0xdd)
			{
				\_SB.PCI0.LPCB.KLSC = 0x01
			}
		}

		\_SB.PCI0.LPCB.KLBC =
			\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLBE))
	}

	/*
	 * Disable ACPI support.
	 * This should always be the last action before entering S4 or S5.
	 */
	\_SB.PCI0.LPCB.EC.OSFG = 0x00
}

Method (RWAK, 1, Serialized)
{
	/*
	 * Enable ACPI support.
	 * This should always be the first action when exiting S4 or S5.
	 */
	\_SB.PCI0.LPCB.EC.OSFG = 0x01

	/* Restore EC settings from CMOS */
	\_SB.PCI0.LPCB.EC.TPLE = \_SB.PCI0.LPCB.TPLC
	\_SB.PCI0.LPCB.EC.FLKE = \_SB.PCI0.LPCB.FLKC
	\_SB.PCI0.LPCB.EC.KLSE = \_SB.PCI0.LPCB.KLSC
	\_SB.PCI0.LPCB.EC.KLBE = \_SB.PCI0.LPCB.KLBC
}
