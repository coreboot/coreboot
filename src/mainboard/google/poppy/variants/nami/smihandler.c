/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>

#include "gpio.h"

#define TOUCH_DISABLE         GPP_C3
#define TOUCH_RESET           GPP_B3
#define TOUCH_ENABLE          GPP_B4
#define FP_PWR_ENABLE         GPP_B11

/*
 * Elan touchscreen has higher delay requirements than the other
 * devices, so using that.
 */
#define ELAN_STOP_OFF_DELAY   2
#define ELAN_RESET_OFF_DELAY  2
#define ELAN_ENABLE_OFF_DELAY 100

void variant_smi_sleep(u8 slp_typ)
{
	if (slp_typ == ACPI_S5) {
		/* TOUCHSCREEN_DIS# */
		gpio_set(TOUCH_DISABLE, 0);
		mdelay(ELAN_STOP_OFF_DELAY);
		/* TOUCHSCREEN_RST# */
		gpio_set(TOUCH_RESET, 0);
		mdelay(ELAN_RESET_OFF_DELAY);
		/* EN_PP3300_DX_TOUCHSCREEN */
		gpio_set(TOUCH_ENABLE, 0);
		mdelay(ELAN_ENABLE_OFF_DELAY);
		/* PCH_FP_PWR_EN */
		gpio_set(FP_PWR_ENABLE, 0);
	}
}
