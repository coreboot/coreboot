/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.5
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/usb.h>

void mtk_usb_prepare(void)
{
	gpio_output(GPIO(USB_DRVVBUS_P1), 1);
}
