/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <soc/gpio.h>
#include <soc/usb.h>

#define MSDC0_DRV_MASK	0x3fffffff
#define MSDC1_DRV_MASK	0x3ffff000
#define MSDC0_DRV_VALUE	0x24924924
#define MSDC1_DRV_VALUE	0x24924000

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

	/* set sdcard cmd/dat/clk pins driving to 10mA */
	clrsetbits32(gpio_base, MSDC1_DRV_MASK, MSDC1_DRV_VALUE);
}

static void mainboard_init(struct device *dev)
{
	configure_emmc();
	configure_sdcard();
	setup_usb_host();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
