/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <gpio.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

void variant_ramstage_init(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case 102:
	case 104:
	case 107:
	case 109:
	case 115:
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
		break;
	default:
		/* SKU does not have FP Sensor, do not enable FPMCU */
		break;
	}
}
