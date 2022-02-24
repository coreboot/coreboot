/* SPDX-License-Identifier: GPL-2.0-only */

Method (RPTS, 1, NotSerialized)
{
	\_SB.PCI0.LPCB.EC.OSFG = 0x00

	If ((Arg0 == 0x04) || (Arg0 == 0x05))
	{
		/* Store current EC settings in CMOS */
		\_SB.PCI0.LPCB.TPLC =
			\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.TPLE))
		\_SB.PCI0.LPCB.FLKC =
			\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.FLKE))
		\_SB.PCI0.LPCB.KLSC =
			\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLSE))
		\_SB.PCI0.LPCB.KLBC =
			\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLBE))
	}
}

Method (RWAK, 1, Serialized)
{
	\_SB.PCI0.LPCB.EC.OSFG = 0x01

	/* Restore EC settings from CMOS */
	\_SB.PCI0.LPCB.EC.TPLE = \_SB.PCI0.LPCB.TPLC
	\_SB.PCI0.LPCB.EC.FLKE = \_SB.PCI0.LPCB.FLKC
	\_SB.PCI0.LPCB.EC.KLSE = \_SB.PCI0.LPCB.KLSC
	\_SB.PCI0.LPCB.EC.KLBE = \_SB.PCI0.LPCB.KLBC
}
