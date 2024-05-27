/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x6E
#define EC_GPE_SWI 0x6B
#include <ec/dasharo/ec/acpi/ec.asl>

Scope (\_SB) {
	Scope (PCI0) {
		#include "backlight.asl"
	}
}
