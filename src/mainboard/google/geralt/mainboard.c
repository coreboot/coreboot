/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <device/device.h>
#include <soc/bl31.h>
#include <soc/msdc.h>
#include <soc/usb.h>

#include "display.h"
#include "gpio.h"

static void mainboard_init(struct device *dev)
{
	if (display_init_required()) {
		if (configure_display() < 0)
			printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
	} else {
		printk(BIOS_INFO, "%s: Skipped display initialization\n", __func__);
	}

	mtk_msdc_configure_emmc(true);
	mtk_msdc_configure_sdcard();
	setup_usb_host();

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_AP_EC_WARM_RST_REQ.id, true);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
