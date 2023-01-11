/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/bl31.h>
#include <soc/ddp.h>
#include <soc/dpm.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mtcmos.h>
#include <soc/regulator.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"

#define MSDC0_BASE	0x11f60000
#define MSDC0_TOP_BASE	0x11f50000

#define MSDC0_DRV_MASK	0x3fffffff
#define MSDC1_DRV_MASK	0x3ffff000
#define MSDC0_DRV_VALUE	0x24924924
#define MSDC1_DRV_VALUE	0x1b6db000

#define MSDC1_GPIO_MODE0_BASE	0x10005360
#define MSDC1_GPIO_MODE0_MASK	0x77777000
#define MSDC1_GPIO_MODE0_VALUE	0x11111000

#define MSDC1_GPIO_MODE1_BASE	0x10005370
#define MSDC1_GPIO_MODE1_MASK	0x7
#define MSDC1_GPIO_MODE1_VALUE	0x1

/* GPIO names */
#define GPIO_EDPBRDG_INT_ODL		GPIO(EINT6)		/* 6 */
#define GPIO_EDPBRDG_PWREN		GPIO(DSI_TE)		/* 41 */
#define GPIO_EDPBRDG_RST_ODL		GPIO(LCM_RST)		/* 42 */
#define GPIO_EN_PP3300_EDP_DX		GPIO(PERIPHERAL_EN1)	/* 127 */
#define GPIO_EN_PP1800_EDPBRDG_DX	GPIO(PERIPHERAL_EN2)	/* 128 */
#define GPIO_EN_PP1000_EDPBRDG		GPIO(PERIPHERAL_EN3)	/* 129 */
#define GPIO_EN_PP3300_DISPLAY_DX	GPIO(CAM_CLK3)		/* 136 */
#define GPIO_AP_EDP_BKLTEN		GPIO(KPROW1)		/* 152 */
#define GPIO_BL_PWM_1V8			GPIO(DISP_PWM)		/* 40 */

/* Override hs_da_trail for ANX7625 */
void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing)
{
	timing->da_hs_trail += 9;
}

/* Set up backlight control pins as output pin and power-off by default */
static void configure_backlight_and_bridge(void)
{
	/* Disable backlight before turning on bridge */
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
	gpio_output(GPIO_EN_PP3300_DISPLAY_DX, 1);

	/* Turn on bridge */
	gpio_output(GPIO_EDPBRDG_RST_ODL, 0);
	gpio_output(GPIO_EN_PP1000_EDPBRDG, 1);
	gpio_output(GPIO_EN_PP1800_EDPBRDG_DX, 1);
	gpio_output(GPIO_EN_PP3300_EDP_DX, 1);
	mdelay(14);
	gpio_output(GPIO_EDPBRDG_PWREN, 1);
	mdelay(10);
	gpio_output(GPIO_EDPBRDG_RST_ODL, 1);
}

static bool configure_display(void)
{
	struct edid edid;
	const u8 i2c_bus = 3;

	printk(BIOS_INFO, "%s: Starting display init\n", __func__);

	configure_backlight_and_bridge();
	mtk_i2c_bus_init(i2c_bus);

	if (anx7625_init(i2c_bus)) {
		printk(BIOS_ERR, "%s: Can't init ANX7625 bridge\n", __func__);
		return false;
	}

	if (anx7625_dp_get_edid(i2c_bus, &edid)) {
		printk(BIOS_ERR, "%s: Can't get panel's edid\n", __func__);
		return false;
	}

	const char *name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	mtcmos_display_power_on();
	mtcmos_protect_display_bus();

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);
	mtk_ddp_init();
	u32 mipi_dsi_flags = (MIPI_DSI_MODE_VIDEO |
			      MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			      MIPI_DSI_MODE_LPM |
			      MIPI_DSI_MODE_LINE_END |
			      MIPI_DSI_MODE_EOT_PACKET);

	if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid, NULL) < 0) {
		printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
		return false;
	}

	if (anx7625_dp_start(i2c_bus, &edid) < 0) {
		printk(BIOS_ERR, "%s: Can't start display via ANX7625\n", __func__);
		return false;
	}

	mtk_ddp_mode_set(&edid);
	fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	return true;
}

static void configure_audio(void)
{
	/* Audio PWR */
	mtcmos_audio_power_on();

	/* SoC I2S */
	gpio_set_mode(GPIO(I2S3_MCK), PAD_I2S3_MCK_FUNC_I2S3_MCK);
	gpio_set_mode(GPIO(I2S3_BCK), PAD_I2S3_BCK_FUNC_I2S3_BCK);
	gpio_set_mode(GPIO(I2S3_LRCK), PAD_I2S3_LRCK_FUNC_I2S3_LRCK);
	gpio_set_mode(GPIO(I2S3_DO), PAD_I2S3_DO_FUNC_I2S3_DO);
}

static void mainboard_init(struct device *dev)
{
	mtk_msdc_configure_emmc(true);
	mtk_msdc_configure_sdcard();
	configure_audio();
	setup_usb_host();

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_RESET.id, true);

	if (dpm_init())
		printk(BIOS_ERR, "dpm init fail, system can't do DVFS switch\n");

	if (spm_init())
		printk(BIOS_ERR, "spm init fail, system suspend may stuck\n");

	if (display_init_required())
		configure_display();
	else
		printk(BIOS_INFO, "%s: Skipped display init\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
