/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <gpio.h>
#include "gpio_ftns.h"

int get_spd_offset(void)
{
	u8 index = 0;
	/*
	 * One SPD file contains all 4 options, determine which index to
	 * read here, then call into the standard routines.
	 */
	if (gpio_get(GPIO_49))
		index |= 1 << 0;
	if (gpio_get(GPIO_50))
		index |= 1 << 1;

	return index;
}
