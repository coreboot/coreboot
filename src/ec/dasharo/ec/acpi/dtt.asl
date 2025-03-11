/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dptf.h>

Device (TPWR)
{
	Name (_HID, DPTF_TPWR_DEVICE)
	Name (_UID, "TPWR")
	Name (_STR, Unicode("Intel DTT Power Participant"))

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}

	Name (PTYP, 0x11) // Device is a platform power device

	Method (PSRC, 0) // Power source
	{
		Return (^^AC.ACFG)
	}

	Method (ARTG, 0) // Adapter rating
	{
		If (^^PCI0.LPCB.EC0.ECOK)
		{
			Return (^^PCI0.LPCB.EC0.APWR)
		}
		Else
		{
			Return (0)
		}
	}

	Method (PBOK, 1) // Power Boss OK
	{
		// Notify EC to de-assert PROCHOT#
		// Required by DTT, but N/A on this hardware, PROCHOT#
		// is de-asserted by board logic upon AC plug-in
	}
}

Device (TBAT)
{
	Name (_HID, DPTF_BAT1_DEVICE)
	Name (_STR, Unicode("Intel DTT Battery Participant"))

	Method (_STA, 0, NotSerialized)
	{
		Return (^^BAT0._STA)
	}

	Name (PTYP, 0x0c) // Device is a platform battery device
	Name (CTYP, 0x02) // Hybrid Power Boost architecture
}
