/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <bootmode.h>
#include <device/device.h>
#include <device/mmio.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/bl31.h>
#include <soc/display.h>
#include <soc/dpm_v2.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mt6359p.h>
#include <soc/mtcmos.h>
#include <soc/spm_common.h>
#include <soc/storage.h>
#include <soc/usb.h>

#include "gpio.h"
#include "storage.h"

#define AFE_SE_SECURE_CON1	(AUDIO_BASE + 0x5634)

static void configure_rt9123_rt1019(void)
{
	/* SoC I2S */
	gpio_set_mode(GPIO_I2S_SPKR_BCK, GPIO_FUNC(DMIC0_DAT0, I2SOUT1_BCK));
	gpio_set_mode(GPIO_I2S_SPKR_LRCK, GPIO_FUNC(DMIC1_CLK, I2SOUT1_LRCK));
	gpio_set_mode(GPIO_I2S_SPKR_DO, GPIO_FUNC(DMIC1_DAT0, I2SOUT1_DO));

	printk(BIOS_INFO, "%s: AMP configuration done\n", __func__);
}

static void configure_alc5645(void)
{
	/* SoC I2S */
	gpio_set_mode(GPIO_I2S_HP_MCK, GPIO_FUNC(I2SOUT0_MCK, I2SOUT0_MCK));
	gpio_set_mode(GPIO_I2S_HP_BCK, GPIO_FUNC(I2SOUT0_BCK, I2SOUT0_BCK));
	gpio_set_mode(GPIO_I2S_HP_LRCK, GPIO_FUNC(I2SOUT0_LRCK, I2SOUT0_LRCK));
	gpio_set_mode(GPIO_I2S_HP_DO, GPIO_FUNC(I2SOUT0_DO, I2SOUT0_DO));

	/* Init I2C bus timing register for audio codecs */
	mtk_i2c_bus_init(I2C2, I2C_SPEED_STANDARD);

	printk(BIOS_INFO, "%s: AMP configuration done\n", __func__);
}

static void configure_audio(void)
{
	mtcmos_audio_power_on();
	mtcmos_protect_audio_bus();

	/* Switch to normal mode */
	write32p(AFE_SE_SECURE_CON1, 0x0);

	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_RT9123)) ||
	    fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_RT1019)))
		configure_rt9123_rt1019();
	else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_ALC5645)))
		configure_alc5645();
	else
		printk(BIOS_WARNING, "Unknown amp\n");
}

static void power_on_fpmcu(void)
{
	/* Power on the fingerprint MCU */
	gpio_output(GPIO_EN_PWR_FP, 1);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 1);
}

enum mtk_storage_type mainboard_get_storage_type(void)
{
	uint32_t index = storage_id();

	switch (index) {
	case 0:
		return STORAGE_UFS_31;
	case 1:
		return STORAGE_UFS_22;
	case 2:
		return STORAGE_EMMC;
	default:
		printk(BIOS_WARNING, "unsupported storage id %u\n", index);
	}
	return STORAGE_UNKNOWN;
}

static void mainboard_init(struct device *dev)
{
	if (mainboard_get_storage_type() == STORAGE_EMMC) {
		mtk_msdc_configure_emmc(true);
		mtcmos_ufs_power_off();
	}

	dpm_init();
	setup_usb_host();
	setup_usb_secondary_host();
	spm_init();
	power_on_fpmcu();
	configure_audio();

	if (CONFIG(SKYWALKER_SDCARD_INIT))
		mtk_msdc_configure_sdcard();

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_AP_EC_WARM_RST_REQ.id, true);

	if (display_init_required()) {
		if (mtk_display_init() < 0)
			printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
	} else {
		printk(BIOS_INFO, "%s: Skipping display init; disabling secure mode\n",
		       __func__);
		mtcmos_display_power_on();
		mtcmos_protect_display_bus();
		mtk_display_disable_secure_mode();
	}

}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
