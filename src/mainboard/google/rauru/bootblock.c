/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/spi.h>

#include "gpio.h"

static void usb3_hub_reset(void)
{
	gpio_output(GPIO(USB_RST), 1);
}

void bootblock_mainboard_init(void)
{
	mtk_snfc_init();
	usb3_hub_reset();
}
