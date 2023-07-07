/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <baseboard/variants.h>

bool variant_is_half_populated(void)
{
	/*
	 * FIXME: b/290253752 - Memory Capacity is incorrect
	 * MEM_CH_SEL GPIO (GPP_E13) is not working as expected on Ovis as result
	 * channel select configuration is set to single (MC0) instead dual (MC0/1).
	 * Only MC0 is reporting DIMM attached and MC1 is disable.
	 *
	 * W/A: Always report full memory capacity as in dual channel (MC0/1).
	 */
	return false;
}
