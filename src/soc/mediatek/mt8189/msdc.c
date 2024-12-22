/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.8
 */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/msdc.h>
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
