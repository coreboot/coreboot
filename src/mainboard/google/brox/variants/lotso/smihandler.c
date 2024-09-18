/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>

#include "gpio.h"

#define TOUCH_RESET           GPP_F17
#define TOUCH_ENABLE          GPP_F7


#define GOODIX_RESET_OFF_DELAY  5

void variant_smi_sleep(u8 slp_typ)
{
	if (slp_typ == ACPI_S5) {
		/* TOUCHSCREEN_RST# */
		gpio_set(TOUCH_RESET, 0);
		mdelay(GOODIX_RESET_OFF_DELAY);
		/* EN_PP3300_DX_TOUCHSCREEN */
		gpio_set(TOUCH_ENABLE, 0);
	}
}
