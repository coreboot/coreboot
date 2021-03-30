/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x50 /* GPP_E16 */
#define EC_GPE_SWI 0x29 /* GPP_D9 */
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
}

Scope (\_GPE) {
	#include "gpe.asl"
}
