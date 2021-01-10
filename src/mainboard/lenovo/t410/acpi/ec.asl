/* SPDX-License-Identifier: GPL-2.0-only */

Name(\TCRT, 100)
Name(\TPSV, 90)
Name(\FLVL, 0)

#include <ec/lenovo/h8/acpi/ec.asl>

#define H8_BAT_THRESHOLDS_BIT7
#include <ec/lenovo/h8/acpi/thinkpad_bat_thresholds_24.asl>
