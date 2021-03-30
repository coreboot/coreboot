/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SWI 0x49 /* GPP_E15 */
#define EC_GPE_SCI 0x50 /* GPP_E16 */
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
}

Scope (\_GPE) {
	#include "gpe.asl"
}

Scope (\_SB.PCI0)
{
	#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
}

Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/pc/ps2_controller.asl>
}
