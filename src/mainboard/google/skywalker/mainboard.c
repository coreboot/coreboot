/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/device.h>
#include <soc/bl31.h>
#include <soc/dpm_v2.h>
#include <soc/spm_common.h>
#include <soc/usb.h>

#include "gpio.h"

static void mainboard_init(struct device *dev)
{
	dpm_init();
	setup_usb_host();
	spm_init();

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
