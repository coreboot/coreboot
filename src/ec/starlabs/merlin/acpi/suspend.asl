/* SPDX-License-Identifier: GPL-2.0-only */

Method (RPTS, 1, Serialized)
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

	Switch (ToInteger (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLBE))))
	{
		// 0xdd == On   == 0x00
		// 0xcc == Off  == 0x01
		// 0xbb == Low  == 0x02
		// 0xaa == High == 0x03
		Case (0xdd)
		{
			\_SB.PCI0.LPCB.KLBC = 0x00
		}
		Case (0xcc)
		{
			\_SB.PCI0.LPCB.KLBC = 0x01
		}
		Case (0xbb)
		{
			\_SB.PCI0.LPCB.KLBC = 0x02
		}
		Case (0xaa)
		{
			\_SB.PCI0.LPCB.KLBC = 0x03
		}
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
	Switch (ToInteger (\_SB.PCI0.LPCB.TPLC))
	{
		// 0x00 == Enabled    == 0x00
		// 0x00 == Re-enabled == 0x11
		// 0x01 == Disabled   == 0x22
		Case (0x00)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0x00,  RefOf(\_SB.PCI0.LPCB.EC.TPLE))
		}
		Case (0x01)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0x22,  RefOf(\_SB.PCI0.LPCB.EC.TPLE))
		}
	}

	\_SB.PCI0.LPCB.EC.ECWR (\_SB.PCI0.LPCB.FLKC,  RefOf(\_SB.PCI0.LPCB.EC.FLKE))

	Switch (ToInteger (\_SB.PCI0.LPCB.KLSC))
	{
		// 0x00 == Disabled == 0x00
		// 0x01 == Enabled  == 0xdd
		Case (0x00)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0x00,  RefOf(\_SB.PCI0.LPCB.EC.KLSE))
		}
		Case (0x01)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xdd,  RefOf(\_SB.PCI0.LPCB.EC.KLSE))
		}
	}

	Switch (ToInteger (\_SB.PCI0.LPCB.KLBC))
	{
		// 0x00 == On   == 0xdd
		// 0x01 == Off  == 0xcc
		// 0x02 == Low  == 0xbb
		// 0x03 == High == 0xaa
		Case (0x00)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xdd,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0x01)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xcc,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0x02)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xbb,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0x03)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xaa,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
	}
}
