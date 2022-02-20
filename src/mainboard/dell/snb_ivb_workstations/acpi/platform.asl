/* SPDX-License-Identifier: GPL-2.0-only */

Method(_WAK,1)
{
	\_SB.PCI0.LPCB.SIO1.SIOW (Arg0)

	Return(Package(){0,0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.SIO1.SIOS (Arg0)
}

Scope (\_SB)
{
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_UID, 0xAA)
		Name (_STA, 0x0B)

		Name (_PRW, Package() { 8, 3})
	}
}

Scope (\_GPE)
{
	Method (_L08, 0, NotSerialized)
	{
		\_SB.PCI0.LPCB.SIO1.SIOH ()
		Notify (\_SB.PWRB, 0x02)
	}

	Method (_L0D, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.EHC1, 0x02)
		Notify (\_SB.PCI0.EHC2, 0x02)
		Notify (\_SB.PCI0.GLAN, 0x02)
	}

	Method (_L09, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.RP01, 0x02)
		Notify (\_SB.PCI0.RP02, 0x02)
		Notify (\_SB.PCI0.RP03, 0x02)
		Notify (\_SB.PCI0.RP04, 0x02)
		Notify (\_SB.PCI0.RP05, 0x02)
		Notify (\_SB.PCI0.RP06, 0x02)
		Notify (\_SB.PCI0.RP07, 0x02)
		Notify (\_SB.PCI0.RP08, 0x02)
		Notify (\_SB.PCI0.PEGP, 0x02)
	}
}
