/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.8
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/msdc.h>
#include <soc/mt6359p.h>
#include <soc/regulator.h>

static const struct pad_func emmc_pins[] = {
	PAD_FUNC_DOWN(EMMC_CLK, MSDC0_CLK),
	PAD_FUNC_DOWN(EMMC_DSL, MSDC0_DSL),
	PAD_FUNC_UP(EMMC_CMD, MSDC0_CMD),
	PAD_FUNC_UP(EMMC_DAT0, MSDC0_DAT0),
	PAD_FUNC_UP(EMMC_DAT1, MSDC0_DAT1),
	PAD_FUNC_UP(EMMC_DAT2, MSDC0_DAT2),
	PAD_FUNC_UP(EMMC_DAT3, MSDC0_DAT3),
	PAD_FUNC_UP(EMMC_DAT4, MSDC0_DAT4),
	PAD_FUNC_UP(EMMC_DAT5, MSDC0_DAT5),
	PAD_FUNC_UP(EMMC_DAT6, MSDC0_DAT6),
	PAD_FUNC_UP(EMMC_DAT7, MSDC0_DAT7),
};

static const struct pad_func sdcard_pins[] = {
	PAD_FUNC_DOWN(MSDC1_CLK, MSDC1_CLK),
	PAD_FUNC_UP(MSDC1_CMD, MSDC1_CMD),
	PAD_FUNC_UP(MSDC1_DAT0, MSDC1_DAT0),
	PAD_FUNC_UP(MSDC1_DAT1, MSDC1_DAT1),
	PAD_FUNC_UP(MSDC1_DAT2, MSDC1_DAT2),
	PAD_FUNC_UP(MSDC1_DAT3, MSDC1_DAT3),
};

void mtk_msdc_configure_emmc(bool is_early_init)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(emmc_pins); i++) {
		gpio_set_mode(emmc_pins[i].gpio, emmc_pins[i].func);
		gpio_set_pull(emmc_pins[i].gpio, GPIO_PULL_ENABLE, emmc_pins[i].select);
		gpio_set_driving(emmc_pins[i].gpio, GPIO_DRV_6_MA);
	}

	if (is_early_init)
		mtk_emmc_early_init((void *)MSDC0_BASE, (void *)MSDC0_TOP_BASE);
}

void mtk_msdc_configure_sdcard(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(sdcard_pins); i++) {
		gpio_set_mode(sdcard_pins[i].gpio, sdcard_pins[i].func);
		gpio_set_pull(sdcard_pins[i].gpio, GPIO_PULL_ENABLE, sdcard_pins[i].select);
		gpio_set_driving(sdcard_pins[i].gpio, GPIO_DRV_8_MA);
	}

	mt6359p_init_pmif_arb();
	/* enable SD card power */
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMCH, 3000000);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMC, 3000000);
	mainboard_enable_regulator(MTK_REGULATOR_VMCH, true);
	mainboard_enable_regulator(MTK_REGULATOR_VMC, true);
}
