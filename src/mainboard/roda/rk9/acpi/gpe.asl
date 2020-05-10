/* SPDX-License-Identifier: GPL-2.0-only */

Scope (_GPE)
{
	/* The event numbers correspond to the bit numbers in the
	 * GPE0_EN register PMBASE + 0x28.
	 */
	/* IMO we don't need empty Methods here. If we don't specify
	   one and don't set the GPE0_EN bit Linux won't neither enable
	   it. - Nico */

	// Thermal Event - original BIOS doesn't have it
	Method (_L00, 0)
	{
		/* FIXME: We should enable throttling here. */
	}

	// Hot Plug
	Method (_L01, 0)
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

	// USB5
	Method (_L05, 0)
	{

	}

	// _L06 TCOSCI

	// SMBus Wake Status
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

	// GPI8
	Method (_L18, 0)
	{

	}

	// USB6
	Method (_L20, 0)
	{

	}
}
