/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Scope (_GPE)
{
	/* The event numbers correspond to the bit numbers in the
	 * GPE0_EN register PMBASE + 0x2C.
	 */

	// Thermal Event
	Method (_L00, 0)
	{

	}

	// Hot Plug
	Method (_L01, 0)
	{

	}

	// Software GPE
	Method (_L02, 0)
	{

	}

	// USB1
	Method (_L03, 0)
	{

	}

	// USB2
	Method (_L04, 0)
	{

	}

	// AC97
	Method (_L05, 0)
	{

	}

	// _L06 TCOSCI

	// SMBus (Reserved!)
	Method (_L07, 0)
	{

	}

	// COM1/COM2 (RI)
	Method (_L08, 0)
	{

	}

	// PCIe
	Method (_L09, 0)
	{

	}

	// _L0A BatLow / Quick Resume

	// PME
	Method (_L0B, 0)
	{

	}

	// USB3
	Method (_L0C, 0)
	{

	}

	// PME B0
	Method (_L0D, 0)
	{

	}

	// USB4
	Method (_L0E, 0)
	{

	}

	// _L10 - _L1f: GPIn

	// GPI13
	Method (_L1D, 0)
	{

	}
}
