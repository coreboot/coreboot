/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/bl31.h>
#include <soc/display.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"
#include "panel.h"

static void configure_alc1019(void)
{
	mtcmos_audio_power_on();

	/* Set up I2S */
	gpio_set_mode(GPIO(I2S2_MCK), PAD_I2S2_MCK_FUNC_I2S2_MCK);
	gpio_set_mode(GPIO(I2S2_BCK), PAD_I2S2_BCK_FUNC_I2S2_BCK);
	gpio_set_mode(GPIO(I2S2_LRCK), PAD_I2S2_LRCK_FUNC_I2S2_LRCK);
	gpio_set_mode(GPIO(EINT4), PAD_EINT4_FUNC_I2S3_DO);
}

static void configure_alc5645(void)
{
	mtcmos_audio_power_on();

	/* Set up I2S */
	gpio_set_mode(GPIO(I2S1_MCK), PAD_I2S1_MCK_FUNC_I2S1_MCK);
	gpio_set_mode(GPIO(I2S1_BCK), PAD_I2S1_BCK_FUNC_I2S1_BCK);
	gpio_set_mode(GPIO(I2S1_LRCK), PAD_I2S1_LRCK_FUNC_I2S1_LRCK);
	gpio_set_mode(GPIO(I2S1_DO), PAD_I2S1_DO_FUNC_I2S1_DO);

	/* Init I2C bus timing register for audio codecs */
	mtk_i2c_bus_init(I2C5, I2C_SPEED_STANDARD);
}

static void mainboard_init(struct device *dev)
{
	mtk_msdc_configure_emmc(true);

	if (CONFIG(CORSOLA_SDCARD_INIT)) {
		printk(BIOS_INFO, "SD card init\n");

		/* External SD Card connected via USB */
		gpio_output(GPIO_EN_PP3300_SDBRDG_X, 1);
	}

	setup_usb_host();

	if (fw_config_probe(FW_CONFIG(SECONDARY_USB, ENABLED))) {
		/* Change host to USB2 port0 for initialization */
		setup_usb_secondary_host();
	}

	if (!fw_config_is_provisioned() ||
	    fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_ALC1019)))
		configure_alc1019();
	else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_ALC5645)))
		configure_alc5645();

	if (spm_init())
		printk(BIOS_ERR, "spm init failed, system suspend may not work\n");

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_RESET.id, true);

	if (display_init_required()) {
		if (mtk_display_init() < 0)
			printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
	} else {
		if (CONFIG(BOARD_GOOGLE_STARYU_COMMON)) {
			mtk_i2c_bus_init(PMIC_I2C_BUS, I2C_SPEED_FAST);
			if (is_pmic_aw37503(PMIC_I2C_BUS)) {
				printk(BIOS_DEBUG, "Initialize PMIC AW37503\n");
				aw37503_init(PMIC_I2C_BUS);
			}
		}
		printk(BIOS_INFO, "%s: Skipped display init\n", __func__);
	}
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
