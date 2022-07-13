/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.9
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/gpio.h>
#include <soc/regulator.h>
#include <soc/msdc.h>
#include <soc/mt6359p.h>

DEFINE_BITFIELD(MSDC0_DRV_0, 29, 18)
DEFINE_BITFIELD(MSDC0_DRV_1, 17, 0)
DEFINE_BITFIELD(MSDC1_DRV, 26, 9)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_0, 30, 28)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_1, 26, 24)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_2, 22, 20)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_3, 18, 16)
DEFINE_BITFIELD(MSDC1_GPIO_MODE0_4, 14, 12)
DEFINE_BITFIELD(MSDC1_GPIO_MODE1_0, 2, 0)

#define MSDC0_DRV_OFFSET_0 0x0
#define MSDC0_DRV_OFFSET_1 0x10
#define MSDC1_DRV_OFFSET_0 0x0

#define MSDC0_DRV_VALUE_0 0x6db
#define MSDC0_DRV_VALUE_1 0x1b6db
#define MSDC1_DRV_VALUE_0 0x1b6db
#define MSDC1_GPIO_MODE0_VALUE 0x1
#define MSDC1_GPIO_MODE1_VALUE 0x1

enum {
	MSDC1_GPIO_MODE0_BASE = 0x10005440,
	MSDC1_GPIO_MODE1_BASE = 0x10005450,
};

void mtk_msdc_configure_emmc(bool is_early_init)
{
	void *gpio_base = (void *)IOCFG_LT_BASE;
	unsigned int i;

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

	/* set eMMC cmd/dat/clk pins driving to 8mA */
	SET32_BITFIELDS(gpio_base + MSDC0_DRV_OFFSET_0, MSDC0_DRV_0, MSDC0_DRV_VALUE_0);
	SET32_BITFIELDS(gpio_base + MSDC0_DRV_OFFSET_1, MSDC0_DRV_1, MSDC0_DRV_VALUE_1);

	if (is_early_init)
		mtk_emmc_early_init((void *)MSDC0_BASE, (void *)MSDC0_TOP_BASE);
}

void mtk_msdc_configure_sdcard(void)
{
	void *gpio_base = (void *)IOCFG_RT_BASE;
	void *gpio_mode0_base = (void *)MSDC1_GPIO_MODE0_BASE;
	void *gpio_mode1_base = (void *)MSDC1_GPIO_MODE1_BASE;
	unsigned int i;

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

	/* set SD card cmd/dat/clk pins driving to 8mA */
	SET32_BITFIELDS(gpio_base + MSDC1_DRV_OFFSET_0, MSDC1_DRV, MSDC1_DRV_VALUE_0);

	/* set SD card dat2/dat1/dat0/cmd/clk pins to msdc1 mode */
	SET32_BITFIELDS(gpio_mode0_base,
			MSDC1_GPIO_MODE0_0, MSDC1_GPIO_MODE0_VALUE,
			MSDC1_GPIO_MODE0_1, MSDC1_GPIO_MODE0_VALUE,
			MSDC1_GPIO_MODE0_2, MSDC1_GPIO_MODE0_VALUE,
			MSDC1_GPIO_MODE0_3, MSDC1_GPIO_MODE0_VALUE,
			MSDC1_GPIO_MODE0_4, MSDC1_GPIO_MODE0_VALUE);

	/* set SD card dat3 pin to msdc1 mode */
	SET32_BITFIELDS(gpio_mode1_base,
			MSDC1_GPIO_MODE1_0, MSDC1_GPIO_MODE1_VALUE);

	/* enable SD card power */
	mt6359p_init();
	mainboard_enable_regulator(MTK_REGULATOR_VMCH, true);
	mainboard_enable_regulator(MTK_REGULATOR_VMC, true);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMCH, 3000000);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMC, 3000000);
}
