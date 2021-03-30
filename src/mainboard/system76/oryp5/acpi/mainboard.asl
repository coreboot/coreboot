/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x17 /* GPP_B23 */
#define EC_GPE_SWI 0x26 /* GPP_G6 */
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB)
{
	#include "sleep.asl"
}

Scope (\_GPE)
{
	#include "gpe.asl"
}
