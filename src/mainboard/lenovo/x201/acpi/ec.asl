/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/lenovo/h8/acpi/ec.asl>

Scope(\_SB.PCI0.LPCB.EC)
{
}

#define H8_BAT_THRESHOLDS_BIT7
#include <ec/lenovo/h8/acpi/thinkpad_bat_thresholds_24.asl>
