/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <baseboard/variants.h>

void variant_ramstage_init(void)
{
	/*
	 * Enable power to FPMCU, wait for power rail to stabilize,
	 * and then deassert FPMCU reset.
	 * Waiting for the power rail to stabilize can take a while,
	 * a minimum of 400us on Nightfury.
	 */
}
