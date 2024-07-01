/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <device/device.h>
#include <fw_config.h>
#include <soc/bl31.h>
#include <soc/display.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mt6359p.h>
#include <soc/mtcmos.h>
#include <soc/usb.h>

#include "gpio.h"

#define AFE_SE_SECURE_CON	(AUDIO_BASE + 0x17a8)

static void configure_i2s(void)
{
	/* Audio PWR */
	mtcmos_audio_power_on();
	mtcmos_protect_audio_bus();

	/* Switch to normal mode */
	write32p(AFE_SE_SECURE_CON, 0x0);

	/* SoC I2S */
	gpio_set_mode(GPIO_I2SI1_LRCK, PAD_I2SO2_D2_FUNC_TDMIN_LRCK);
	gpio_set_mode(GPIO_I2SI1_BCK, PAD_I2SIN_D3_FUNC_TDMIN_BCK);
	gpio_set_mode(GPIO_I2SO1_D0, PAD_GPIO11_FUNC_I2SO1_D0);
}

static void configure_audio(void)
{
	if (CONFIG(GERALT_USE_NAU8318))
		return;

	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_MAX98390)) ||
	    fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_TAS2563))) {

		mtk_i2c_bus_init(I2C0, I2C_SPEED_FAST);
		configure_i2s();
	}
}

static void mainboard_init(struct device *dev)
{
	mt6359p_init_pmif_arb();

	if (display_init_required()) {
		if (mtk_display_init() < 0)
			printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
	} else {
		printk(BIOS_INFO, "%s: Skipped display initialization\n", __func__);
	}

	mtk_msdc_configure_emmc(true);

	configure_audio();

	if (CONFIG(GERALT_SDCARD_INIT))
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
	.enable_dev = mainboard_enable,
};
