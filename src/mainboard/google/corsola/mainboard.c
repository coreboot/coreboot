/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <soc/msdc.h>
#include <soc/spm.h>
#include <soc/usb.h>

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
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
