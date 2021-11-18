/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/msdc.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

static void register_reset_to_bl31(void)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = { .type = BL_AUX_PARAM_MTK_RESET_GPIO },
		.gpio = { .polarity = ARM_TF_GPIO_LEVEL_HIGH },
	};

	param_reset.gpio.index = GPIO_RESET.id;
	register_bl31_aux_param(&param_reset.h);
}

static void mainboard_init(struct device *dev)
{
	mtk_msdc_configure_emmc(true);

	if (CONFIG(SDCARD_INIT)) {
		printk(BIOS_INFO, "SD card init\n");
		mtk_msdc_configure_sdcard();
	}

	setup_usb_host();

	if (spm_init())
		printk(BIOS_ERR, "spm init failed, system suspend may not work\n");

	register_reset_to_bl31();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
