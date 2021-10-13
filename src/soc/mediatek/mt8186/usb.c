/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.5
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/usb.h>

static void usb3_hub_reset(void)
{
	gpio_output(GPIO(PERIPHERAL_EN2), 1);
}

void mtk_usb_prepare(void)
{
	usb3_hub_reset();
	gpio_output(GPIO(USB_DRVVBUS_P1), 1);
}
