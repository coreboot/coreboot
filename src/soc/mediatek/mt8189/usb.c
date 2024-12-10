/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.5
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/pll_common.h>
#include <soc/usb.h>

void mtk_usb_prepare(void)
{
	/* enable P3 VBUS */
	gpio_output(GPIO(USB3_DRV_VBUS), 1);
}
