/* SPDX-License-Identifier: GPL-2.0-only */

Method (RPTS, 1, NotSerialized)
{
	\_SB.PCI0.LPCB.EC.OSFG = 0x00

	If ((Arg0 == 0x04) || (Arg0 == 0x05))
	{
		/* Store current EC settings */
		\_SB.PCI0.LPCB.EC.TPLA = \_SB.PCI0.LPCB.TPLS
		\_SB.PCI0.LPCB.EC.FLKA = \_SB.PCI0.LPCB.FLKS
		\_SB.PCI0.LPCB.EC.KLBE = \_SB.PCI0.LPCB.KLBC
	}
}

Method (RWAK, 1, Serialized)
{
	\_SB.PCI0.LPCB.EC.OSFG = 0x01
}
