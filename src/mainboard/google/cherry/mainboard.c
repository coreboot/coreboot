/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/mt6360.h>
#include <soc/regulator.h>
#include <soc/usb.h>

DEFINE_BITFIELD(MSDC0_DRV, 29, 0)
DEFINE_BITFIELD(MSDC1_DRV, 17, 0)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_0, 26, 24)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_1, 30, 28)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_0, 2, 0)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_1, 6, 4)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_2, 10, 8)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_3, 14, 12)

#define MSDC0_DRV_VALUE	0x1b6db6db
#define MSDC1_DRV_VALUE	0x1b6db
#define MSDC1_GPIO_MODE0_VALUE	0x1
#define MSDC1_GPIO_MODE1_VALUE	0x1

enum {
	MSDC1_GPIO_MODE0_BASE = 0x100053d0,
	MSDC1_GPIO_MODE1_BASE = 0x100053e0,
};

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

	mtk_i2c_bus_init(7);
	mt6360_init(7);
	mt6360_ldo_enable(MT6360_LDO3, 1);
	mt6360_ldo_enable(MT6360_LDO5, 1);
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
