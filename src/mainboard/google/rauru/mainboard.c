/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/storage.h>
#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/bl31.h>
#include <soc/display.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mt6373.h>
#include <soc/pcie.h>
#include <soc/spm_mtcmos.h>
#include <soc/storage.h>
#include <soc/usb.h>
#include <thread.h>
#include <variants.h>

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
	switch (get_audio_amp_id()) {
	case AUD_AMP_ID_TAS2563:
		mtk_i2c_bus_init(I2C3, I2C_SPEED_FAST);
		configure_tas2563();
		break;
	case AUD_AMP_ID_ALC5645:
		configure_alc5645();
		break;
	case AUD_AMP_ID_NAU8318:
	case AUD_AMP_ID_UNKNOWN:
	default:
		/* No configuration needed for NAU8318 or UNKNOWN */
		break;
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

static struct thread_handle display_thread;
static struct thread_mutex display_thread_start_mutex;

static enum cb_err display_init_thread(void *unused)
{
	/* The display thread takes the mutex to block the main thread from
	   starting soc_init until the display thread is ready.  */
	thread_mutex_lock(&display_thread_start_mutex);

	if (mtk_display_init(&display_thread_start_mutex) < 0) {
		printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
		thread_mutex_unlock(&display_thread_start_mutex);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static void mainboard_final(struct device *dev)
{
	if (!ENV_SUPPORTS_COOP)
		return;
	thread_join(&display_thread);
}

static void mainboard_init(struct device *dev)
{
	mtcmos_dsi_phy_switch_to_sw();

	if (display_init_required()) {
		enable_display_power_rail();
		if (ENV_SUPPORTS_COOP) {
			if (thread_run(&display_thread, display_init_thread, NULL))
				die("%s: Failed to start display init thread\n", __func__);
		} else {
			if (mtk_display_init(NULL) < 0)
				printk(BIOS_ERR, "%s: Failed to init display\n", __func__);

		}
	} else {
		printk(BIOS_INFO, "%s: Skipped display initialization\n", __func__);
	}

	/* The main thread waits here until the display thread reaches the MIPI
	   delay window and unlocks this mutex. */
	thread_mutex_lock(&display_thread_start_mutex);
	thread_mutex_unlock(&display_thread_start_mutex);

	setup_usb_host();
	power_on_fpmcu();
	configure_audio();

	if (CONFIG(RAURU_SDCARD_INIT))
		mtk_msdc_configure_sdcard();

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_AP_EC_WARM_RST_REQ.id, true);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
	dev->ops->final = &mainboard_final;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
