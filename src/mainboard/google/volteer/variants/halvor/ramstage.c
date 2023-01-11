/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <gpio.h>
#include <baseboard/variants.h>

void variant_ramstage_init(void)
{
	/*
	 * Assert FPMCU reset and enable power to FPMCU,
	 * wait for power rail to stabilize,
	 * and then deassert FPMCU reset.
	 * Waiting for the power rail to stabilize can take a while.
	 */
	gpio_output(GPP_C23, 0);
	gpio_output(GPP_A21, 1);
	mdelay(1);
	gpio_output(GPP_C23, 1);
}
