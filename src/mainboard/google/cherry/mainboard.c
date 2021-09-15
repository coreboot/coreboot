/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dpm.h>
#include <soc/dptx.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mt6360.h>
#include <soc/mtcmos.h>
#include <soc/regulator.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

/* GPIO to schematics names */
#define GPIO_AP_EDP_BKLTEN GPIO(DGI_D5)
#define GPIO_BL_PWM_1V8 GPIO(DISP_PWM0)
#define GPIO_EDP_HPD_1V8 GPIO(GPIO_07)
#define GPIO_EN_PP3300_DISP_X GPIO(I2SO1_D2)

DEFINE_BITFIELD(MSDC0_DRV, 29, 0)
DEFINE_BITFIELD(MSDC1_DRV, 17, 0)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_0, 26, 24)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_1, 30, 28)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_0, 2, 0)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_1, 6, 4)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_2, 10, 8)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_3, 14, 12)

#define MSDC0_BASE	0x11230000
#define MSDC0_TOP_BASE	0x11f50000

#define MSDC0_DRV_VALUE	0x1b6db6db
#define MSDC1_DRV_VALUE	0x1b6db
#define MSDC1_GPIO_MODE0_VALUE	0x1
#define MSDC1_GPIO_MODE1_VALUE	0x1

enum {
	MSDC1_GPIO_MODE0_BASE = 0x100053d0,
	MSDC1_GPIO_MODE1_BASE = 0x100053e0,
};

static void register_reset_to_bl31(void)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = { .type = BL_AUX_PARAM_MTK_RESET_GPIO },
		.gpio = { .polarity = ARM_TF_GPIO_LEVEL_HIGH },
	};

	param_reset.gpio.index = GPIO_RESET.id;
	register_bl31_aux_param(&param_reset.h);
}

static void configure_emmc(void)
{
	void *gpio_base = (void *)IOCFG_TL_BASE;
	int i;

	const gpio_t emmc_pu_pin[] = {
		GPIO(EMMC_DAT0), GPIO(EMMC_DAT1),
		GPIO(EMMC_DAT2), GPIO(EMMC_DAT3),
		GPIO(EMMC_DAT4), GPIO(EMMC_DAT5),
		GPIO(EMMC_DAT6), GPIO(EMMC_DAT7),
		GPIO(EMMC_CMD), GPIO(EMMC_RSTB),
	};

	const gpio_t emmc_pd_pin[] = {
		GPIO(EMMC_DSL), GPIO(EMMC_CLK),
	};

	for (i = 0; i < ARRAY_SIZE(emmc_pu_pin); i++)
		gpio_set_pull(emmc_pu_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_UP);

	for (i = 0; i < ARRAY_SIZE(emmc_pd_pin); i++)
		gpio_set_pull(emmc_pd_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_DOWN);

	/* set eMMC cmd/dat/clk/ds/rstb pins driving to 8mA */
	SET32_BITFIELDS(gpio_base, MSDC0_DRV, MSDC0_DRV_VALUE);

	mtk_emmc_early_init((void *)MSDC0_BASE, (void *)MSDC0_TOP_BASE);
}

static void configure_sdcard(void)
{
	void *gpio_base = (void *)IOCFG_RB_BASE;
	void *gpio_mode0_base = (void *)MSDC1_GPIO_MODE0_BASE;
	void *gpio_mode1_base = (void *)MSDC1_GPIO_MODE1_BASE;
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
	SET32_BITFIELDS(gpio_base, MSDC1_DRV, MSDC1_DRV_VALUE);

	/* set sdcard dat2/dat0/dat3/cmd/clk pins to msdc1 mode */
	SET32_BITFIELDS(gpio_mode0_base,
			MSDC1_GPIO_MODE0_0, MSDC1_GPIO_MODE0_VALUE,
			MSDC1_GPIO_MODE0_1, MSDC1_GPIO_MODE0_VALUE);

	/* set sdcard dat1 pin to msdc1 mode */
	SET32_BITFIELDS(gpio_mode1_base,
			MSDC1_GPIO_MODE1_0, MSDC1_GPIO_MODE1_VALUE,
			MSDC1_GPIO_MODE1_1, MSDC1_GPIO_MODE1_VALUE,
			MSDC1_GPIO_MODE1_2, MSDC1_GPIO_MODE1_VALUE,
			MSDC1_GPIO_MODE1_3, MSDC1_GPIO_MODE1_VALUE);

	mtk_i2c_bus_init(I2C7, I2C_SPEED_FAST);

	if (CONFIG(BOARD_GOOGLE_CHERRY))
		mt6360_init(I2C7);

	mainboard_enable_regulator(MTK_REGULATOR_VCCQ, 1);
	mainboard_enable_regulator(MTK_REGULATOR_VCC, 1);
}

/* Set up backlight control pins as output pin and power-off by default */
static void configure_panel_backlight(void)
{
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
}

static void power_on_panel(void)
{
	/* Default power sequence for most panels. */
	gpio_set_pull(GPIO_EDP_HPD_1V8, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_mode(GPIO_EDP_HPD_1V8, 2);
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
}

static bool configure_display(void)
{
	struct edid edid;
	struct fb_info *info;
	const char *name;

	printk(BIOS_INFO, "%s: Starting display initialization\n", __func__);

	mtcmos_display_power_on();
	mtcmos_protect_display_bus();
	configure_panel_backlight();
	power_on_panel();

	mtk_ddp_init();
	mdelay(200);

	if (mtk_edp_init(&edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to initialize eDP\n", __func__);
		return false;
	}
	name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	mtk_ddp_mode_set(&edid);
	info = fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	if (info)
		fb_set_orientation(info, LB_FB_ORIENTATION_NORMAL);

	return true;
}

static void mainboard_init(struct device *dev)
{
	if (display_init_required())
		configure_display();
	else
		printk(BIOS_INFO, "%s: Skipped display initialization\n", __func__);

	configure_emmc();
	configure_sdcard();
	setup_usb_host();

	/* for audio usage */
	if (CONFIG(CHERRY_USE_RT1011))
		mtk_i2c_bus_init(I2C2, I2C_SPEED_FAST);

	if (dpm_init())
		printk(BIOS_ERR, "dpm init failed, DVFS may not work\n");

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
