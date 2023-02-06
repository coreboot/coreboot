/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/gma/acpi/gma.asl>

Scope (GFX0)
{
	Name (BRIG, Package (22) {
		40, /* default AC */
		40, /* default Battery */
		5,
		10,
		15,
		20,
		25,
		30,
		35,
		40,
		45,
		50,
		55,
		60,
		65,
		70,
		75,
		80,
		85,
		90,
		95,
		100
	})
}
