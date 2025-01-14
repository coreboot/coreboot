/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 10.3
 */

#include <gpio.h>
#include <soc/msdc.h>
#include <soc/mt6373.h>
#include <soc/regulator.h>

static const struct pad_func sdcard_pins[] = {
	PAD_FUNC_DOWN(MSDC1_CLK, MSDC1_CLK),
	PAD_FUNC_UP(MSDC1_CMD, MSDC1_CMD),
	PAD_FUNC_UP(MSDC1_DAT0, MSDC1_DAT0),
	PAD_FUNC_UP(MSDC1_DAT1, MSDC1_DAT1),
	PAD_FUNC_UP(MSDC1_DAT2, MSDC1_DAT2),
	PAD_FUNC_UP(MSDC1_DAT3, MSDC1_DAT3),
};

void mtk_msdc_configure_sdcard(void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(sdcard_pins); i++) {
		gpio_set_mode(sdcard_pins[i].gpio, sdcard_pins[i].func);
		gpio_set_pull(sdcard_pins[i].gpio, GPIO_PULL_ENABLE, sdcard_pins[i].select);
		gpio_set_driving(sdcard_pins[i].gpio, GPIO_DRV_6_MA);
	}

	/* enable SD card power */
	mt6373_init_pmif_arb();
	mainboard_enable_regulator(MTK_REGULATOR_VMCH, true);
	mainboard_enable_regulator(MTK_REGULATOR_VMC, true);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMCH, 3000000);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMC, 3000000);
}
