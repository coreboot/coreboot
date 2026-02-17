/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <gpio.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

void variant_ramstage_init(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case 0xB0012:
	case 0xB0014:
	case 0xB0015:
	case 0xB0016:
	case 0xB0021:
	case 0xB0023:
	case 0xB0027:
	case 0xB0032:
	case 0xB0036:
	case 0xB0037:
	case 0xB0038:
	case 0xB0040:
	case 0xB0041:
	case 0xB0042:
	case 0xB0043:
	case 0xB0047:
	case 0xB0048:
		/* SKU does not have FP Sensor, do not enable FPMCU */
		break;
	default:
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
	}
}
