/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Scope (_GPE)
{
	/* The event numbers correspond to the bit numbers in the
	 * GPE0_EN register PMBASE + 0x2C.
	 */

	// Hot Plug
	Method (_L01, 0)
	{
		// TODO
	}

	// Software GPE
	Method (_L02, 0)
	{
		Store (0, GPEC)
	}

	// USB1
	Method (_L03, 0)
	{
		Notify (\_SB.PCI0.USB1, 2)
	}

	// USB2
	Method (_L04, 0)
	{
		Notify (\_SB.PCI0.USB2, 2)
	}

	// AC97
	Method (_L05, 0)
	{
		Notify (\_SB.PCI0.MODM, 2)
		Notify (\_SB.PCI0.HDEF, 2)
	}

	// _L06 TCOSCI

	// SMBus (Reserved!)
	Method (_L07, 0)
	{
		// Store (0x20, \_SB.PCI0.SBUS.HSTS)
	}

	// COM1/COM2 (RI)
	Method (_L08, 0)
	{
		// Don't care
	}

	// PCIe
	Method (_L09, 0)
	{
		// TODO
	}

	// _L0A BatLow / Quick Resume

	// PME
	Method (_L0B, 0)
	{
		Notify (\_SB.PCI0.PCIB.LANR, 0x02)
	}

	// USB3
	Method (_L0C, 0)
	{
		Notify(\_SB.PCI0.USB3, 2)
	}

	// PME B0
	Method (_L0D, 0)
	{
		Notify(\_SB.PCI0.EHC1, 2)
	}

	// USB4
	Method (_L0E, 0)
	{
		Notify(\_SB.PCI0.USB4, 2)
	}
}
