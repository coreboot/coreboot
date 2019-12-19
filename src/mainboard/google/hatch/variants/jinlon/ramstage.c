/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <delay.h>
#include <gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>

void variant_ramstage_init(void)
{
	/*
	 * Enable power to FPMCU, wait for power rail to stabilize,
	 * and then deassert FPMCU reset.
	 * Waiting for the power rail to stabilize can take a while,
	 * a minimum of 400us on Kohaku.
	 */
	gpio_output(GPP_C11, 1);
	mdelay(1);
	gpio_output(GPP_A12, 1);
}
