/* SPDX-License-Identifier: GPL-2.0-only */


/* Board powers on with button or PME# from on-board GbE wake-on-lan.
 * Board shuts down to S5/G2. Any other power management is untested.
 */

Name (\_S0, Package () { 0x00, 0x00, 0x00, 0x00 })
Name (\_S1, Package () { 0x01, 0x01, 0x00, 0x00 })
Name (\_S3, Package () { 0x05, 0x05, 0x00, 0x00 })
Name (\_S4, Package () { 0x06, 0x06, 0x00, 0x00 })
Name (\_S5, Package () { 0x07, 0x07, 0x00, 0x00 })

Scope (\_GPE)
{
	Method (_L03, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB0, 0x02)
	}
	Method (_L04, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB1, 0x02)
	}

	/* WOL header */
	Method (_L08, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.PCI5, 0x02)
		Notify (\_SB.SLBT, 0x02)
	}

	/* PME# */
	Method (_L0B, 0, NotSerialized)
	{
		Notify (\_SB.LID0, 0x02)
	}

	Method (_L0C, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB2, 0x02)
	}

	/* PME_B0_STS# */
	Method (_L0D, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.USB3, 0x02)
	}
}

/* Clear power buttons */
Method (\_INI, 0, NotSerialized)
{
	Or (\_SB.PCI0.ICH0.PS1H, 0x09, \_SB.PCI0.ICH0.PS1H)
	Or (\_SB.PCI0.ICH0.PE1H, 0x01, \_SB.PCI0.ICH0.PE1H)
}

/* Prepare To Sleep */
Method (\_PTS, 1, NotSerialized)
{
	Or (\_SB.PCI0.ICH0.GS0H, 0x19, \_SB.PCI0.ICH0.GS0H)
	Or (\_SB.PCI0.ICH0.GS0L, 0x11, \_SB.PCI0.ICH0.GS0L)
}

/* System Wake */
Method (\_WAK, 1, NotSerialized)
{
	Or (\_SB.PCI0.ICH0.GS0H, 0x19, \_SB.PCI0.ICH0.GS0H)
	Or (\_SB.PCI0.ICH0.GS0L, 0x11, \_SB.PCI0.ICH0.GS0L)

	Return ( Package() { 0x0, 0x0 } )
}
