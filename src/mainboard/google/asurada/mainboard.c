/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <drivers/analogix/anx7625/anx7625.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dpm.h>
#include <soc/dsi.h>
#include <soc/gpio.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mtcmos.h>
#include <soc/regulator.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

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

static void register_reset_to_bl31(void)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = { .type = BL_AUX_PARAM_MTK_RESET_GPIO },
		.gpio = { .polarity = ARM_TF_GPIO_LEVEL_HIGH },
	};

	param_reset.gpio.index = GPIO_RESET.id;
	register_bl31_aux_param(&param_reset.h);
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
	mdelay(2);
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
	if (anx7625_dp_start(i2c_bus, &edid) < 0) {
		printk(BIOS_ERR, "%s: Can't start display via ANX7625\n", __func__);
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
			      MIPI_DSI_MODE_EOT_PACKET);

	if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid, NULL) < 0) {
		printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
		return false;
	}
	mtk_ddp_mode_set(&edid);
	fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	return true;
}

static void configure_emmc(void)
{
	void *gpio_base = (void *)IOCFG_TL_BASE;
	int i;

	const gpio_t emmc_pu_pin[] = {
		GPIO(MSDC0_DAT0), GPIO(MSDC0_DAT1),
		GPIO(MSDC0_DAT2), GPIO(MSDC0_DAT3),
		GPIO(MSDC0_DAT4), GPIO(MSDC0_DAT5),
		GPIO(MSDC0_DAT6), GPIO(MSDC0_DAT7),
		GPIO(MSDC0_CMD), GPIO(MSDC0_RSTB),
	};

	const gpio_t emmc_pd_pin[] = {
		GPIO(MSDC0_DSL), GPIO(MSDC0_CLK),
	};

	for (i = 0; i < ARRAY_SIZE(emmc_pu_pin); i++)
		gpio_set_pull(emmc_pu_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_UP);

	for (i = 0; i < ARRAY_SIZE(emmc_pd_pin); i++)
		gpio_set_pull(emmc_pd_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_DOWN);

	/* set eMMC cmd/dat/clk/ds/rstb pins driving to 10mA */
	clrsetbits32(gpio_base, MSDC0_DRV_MASK, MSDC0_DRV_VALUE);

	mtk_emmc_early_init((void *)MSDC0_BASE, (void *)MSDC0_TOP_BASE);
}

static void configure_sdcard(void)
{
	void *gpio_base = (void *)IOCFG_RM_BASE;
	void *gpio_mode0_base = (void *)MSDC1_GPIO_MODE0_BASE;
	void *gpio_mode1_base = (void *)MSDC1_GPIO_MODE1_BASE;
	uint8_t enable = 1;
	int i;

	const gpio_t sdcard_pu_pin[] = {
		GPIO(MSDC1_DAT0), GPIO(MSDC1_DAT1),
		GPIO(MSDC1_DAT2), GPIO(MSDC1_DAT3),
		GPIO(MSDC1_CMD),
	};

	const gpio_t sdcard_pd_pin[] = {
		GPIO(MSDC1_CLK),
	};

	for (i = 0; i < ARRAY_SIZE(sdcard_pu_pin); i++)
		gpio_set_pull(sdcard_pu_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_UP);

	for (i = 0; i < ARRAY_SIZE(sdcard_pd_pin); i++)
		gpio_set_pull(sdcard_pd_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_DOWN);

	/* set sdcard cmd/dat/clk pins driving to 8mA */
	clrsetbits32(gpio_base, MSDC1_DRV_MASK, MSDC1_DRV_VALUE);

	/* set sdcard dat2/dat0/dat3/cmd/clk pins to msdc1 mode */
	clrsetbits32(gpio_mode0_base, MSDC1_GPIO_MODE0_MASK, MSDC1_GPIO_MODE0_VALUE);

	/* set sdcard dat1 pin to msdc1 mode */
	clrsetbits32(gpio_mode1_base, MSDC1_GPIO_MODE1_MASK, MSDC1_GPIO_MODE1_VALUE);

	mainboard_enable_regulator(MTK_REGULATOR_VCC, enable);
	mainboard_enable_regulator(MTK_REGULATOR_VCCQ, enable);
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
	configure_emmc();
	configure_sdcard();
	configure_audio();
	setup_usb_host();

	register_reset_to_bl31();

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
