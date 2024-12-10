/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/bl31.h>
#include <soc/display.h>
#include <soc/dpm_v2.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mt6373.h>
#include <soc/pcie.h>
#include <soc/spm_common.h>
#include <soc/storage.h>
#include <soc/usb.h>

#include "gpio.h"
#include "storage.h"

#define AFE_SE_SECURE_CON1	(AUDIO_BASE + 0x5634)

static void configure_tas2563(void)
{
	/* Switch to normal mode */
	write32p(AFE_SE_SECURE_CON1, 0x0);

	/* SoC I2S */
	gpio_set_mode(GPIO_I2SI4_BCK, GPIO_FUNC(I2SIN1_BCK, I2SIN4_BCK));
	gpio_set_mode(GPIO_I2SI4_LRCK, GPIO_FUNC(I2SIN1_LRCK, I2SIN4_LRCK));
	gpio_set_mode(GPIO_I2SO4_D0, GPIO_FUNC(I2SOUT1_DO, I2SOUT4_DATA0));

	printk(BIOS_INFO, "%s: done\n", __func__);
}

static void configure_alc5645(void)
{
	/* Switch to normal mode */
	write32p(AFE_SE_SECURE_CON1, 0x0);

	/* Set up I2S */
	gpio_set_mode(GPIO_I2SI6_MCK, GPIO_FUNC(I2SIN0_MCK, I2S_MCK0));
	gpio_set_mode(GPIO_I2SI6_BCK, GPIO_FUNC(I2SIN0_BCK, I2SIN6_0_BCK));
	gpio_set_mode(GPIO_I2SI6_LRCK, GPIO_FUNC(I2SIN0_LRCK, I2SIN6_0_LRCK));
	gpio_set_mode(GPIO_I2SO6_D0, GPIO_FUNC(I2SOUT0_DO, I2SOUT6_0_DO));

	/* Init I2C bus timing register for audio codecs */
	mtk_i2c_bus_init(I2C3, I2C_SPEED_STANDARD);

	printk(BIOS_INFO, "%s: done\n", __func__);
}
static void configure_audio(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_TAS2563))) {
		mtk_i2c_bus_init(I2C3, I2C_SPEED_FAST);
		configure_tas2563();
	} else if (fw_config_probe(FW_CONFIG(AUDIO_AMP, AMP_ALC5645))) {
		configure_alc5645();
	} else {
		printk(BIOS_INFO, "Audio configure default amps NAU8318\n");
	}

	printk(BIOS_INFO, "%s: done\n", __func__);
}

static void power_on_fpmcu(void)
{
	/* Power on the fingerprint MCU */
	gpio_output(GPIO_EN_PWR_FP, 1);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 1);
}

static void enable_display_power_rail(void)
{
	mt6373_init_pmif_arb();
	mt6373_enable_vcn33_3(true);
	mt6373_set_vcn33_3_voltage(3300000);
}

enum mtk_storage_type mainboard_get_storage_type(void)
{
	uint32_t index = storage_id();

	switch (index) {
	case 0:
		return STORAGE_UFS_40;
	case 1:
		return STORAGE_UFS_31;
	case 2:
		return STORAGE_UFS_40_HS;
	case 3:
		return STORAGE_NVME;
	default:
		printk(BIOS_WARNING, "unsupported storage id %u\n", index);
	}
	return STORAGE_UNKNOWN;
}

bool mainboard_needs_pcie_init(void)
{
	return mainboard_get_storage_type() == STORAGE_NVME;
}

static void mainboard_init(struct device *dev)
{
	if (display_init_required()) {
		enable_display_power_rail();
		if (mtk_display_init() < 0)
			printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
	} else {
		printk(BIOS_INFO, "%s: Skipped display initialization\n", __func__);
	}

	setup_usb_host();
	power_on_fpmcu();
	configure_audio();

	if (CONFIG(RAURU_SDCARD_INIT))
		mtk_msdc_configure_sdcard();

	if (dpm_init())
		printk(BIOS_ERR, "dpm init failed, DVFS may not work\n");

	if (spm_init())
		printk(BIOS_ERR, "spm init failed, Suspend may not work\n");

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
