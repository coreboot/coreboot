/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <onboard.h>

Scope (\_SB)
{
	Device (LID0)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Method(_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.LIDS, \LIDS)
			Return (\LIDS)
		}

		// There is no GPIO for LID, the EC pulses WAKE# pin instead.
		// There is no GPE for WAKE#, so fake it with PCI_EXP_WAKE
		Name (_PRW, Package(){ 0x69, 5 }) // PCI_EXP
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}
}

#include <variant/acpi/mainboard.asl>
