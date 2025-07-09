/* SPDX-License-Identifier: GPL-2.0-only */

#define BRIGHTNESS_UP \_SB.PCI0.GFX0.INCB
#define BRIGHTNESS_DOWN \_SB.PCI0.GFX0.DECB
#define THINKPAD_EC_GPE 22

Name(\TCRT, 100)
Name(\TPSV, 90)
Name(\FLVL, 0)

#include <ec/lenovo/h8/acpi/ec.asl>
#include <ec/lenovo/h8/acpi/thinkpad_bat_thresholds_b0.asl>
