/* SPDX-License-Identifier: GPL-2.0-only */

Device (LID0)
{
	Name(_HID, EisaId("PNP0C0D"))
	Method(_LID, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.LIDS) {
			Return (0)
		} Else {
			Return (1)
		}
	}
}

Device (SLPB)
{
	Name(_HID, EisaId("PNP0C0E"))
}

Device (PWRB)
{
	Name(_HID, EisaId("PNP0C0C"))

	// Wake
	Name(_PRW, Package(){0x1d, 0x04})
}

#include "acpi/battery.asl"
