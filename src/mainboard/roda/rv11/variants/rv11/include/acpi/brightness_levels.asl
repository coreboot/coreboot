/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <drivers/intel/gma/acpi/gma.asl>

Scope (GFX0)
{
	Name (BRIG, Package (13)
	{
		 74, /* default AC */
		 74, /* default Battery */
		  9, /*  4 / 46 */
		 11, /*  5 / 46 */
		 15, /*  6 / 46 */
		 20, /*  9 / 46 */
		 24, /* 11 / 46 */
		 28, /* 12 / 46 */
		 39, /* 17 / 46 */
		 50, /* 23 / 46 */
		 60, /* 27 / 46 */
		 74, /* 34 / 46 */
		100, /* 46 / 46 */
	})
}
