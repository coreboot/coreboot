/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <soc/dpm.h>
#include <soc/gpio.h>
#include <soc/regulator.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

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

static void mainboard_init(struct device *dev)
{
	configure_emmc();
	configure_sdcard();
	setup_usb_host();

	register_reset_to_bl31();

	if (dpm_init())
		printk(BIOS_ERR, "dpm init fail, system can't do DVFS switch\n");

	if (spm_init())
		printk(BIOS_ERR, "spm init fail, system suspend may stuck\n");
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
