/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.5
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/infracfg.h>
#include <soc/pll_common.h>
#include <soc/usb.h>

void mtk_usb_prepare(void)
{
	mt_pll_set_usb_clock();

	/* usb drvvbus for 5v power */
	gpio_output(GPIO(USB1_DRV_VBUS), 1);
}
