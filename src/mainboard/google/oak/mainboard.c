/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cache.h>
#include <arch/io.h>
#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <delay.h>
#include <device/device.h>

#include <gpio.h>
#include <soc/mt6391.h>
#include <soc/mtcmos.h>
#include <soc/pinmux.h>
#include <soc/pll.h>

static void configure_audio(void)
{
	mtcmos_audio_power_on();

	/* regulator for codecs */
	switch (board_id()) {
	case 0:
		/* vgp1 set to 1.22V */
		mt6391_configure_ldo(LDO_VCAMD, LDO_1P22);
		/* vgp4 set to 1.8V */
		mt6391_configure_ldo(LDO_VGP4, LDO_1P8);
		break;
	default:
		/* board from Rev1 */
		/* vgp1 set to 1.8V */
		mt6391_configure_ldo(LDO_VCAMD, LDO_1P8);
		/* delay 1ms for realtek's power sequence request */
		mdelay(1);
		/* vcama set to 1.8V */
		mt6391_configure_ldo(LDO_VCAMA, LDO_1P8);
		break;
	}

	/* reset ALC5676 */
	gpio_output(PAD_LCM_RST, 1);

	/* SoC I2S */
	gpio_set_mode(PAD_I2S0_LRCK, PAD_I2S0_LRCK_FUNC_I2S1_WS);
	gpio_set_mode(PAD_I2S0_BCK, PAD_I2S0_BCK_FUNC_I2S1_BCK);
	gpio_set_mode(PAD_I2S0_MCK, PAD_I2S0_MCK_FUNC_I2S1_MCK);
	gpio_set_mode(PAD_I2S0_DATA0, PAD_I2S0_DATA0_FUNC_I2S1_DO_1);
	gpio_set_mode(PAD_I2S0_DATA1, PAD_I2S0_DATA1_FUNC_I2S2_DI_2);

	/* codec ext MCLK ON */
	mt6391_gpio_output(MT6391_KP_COL4, 1);
	mt6391_gpio_output(MT6391_KP_COL5, 1);

	/* set I2S clock to 48KHz */
	mt_pll_set_aud_div(48 * KHz);
}

static void mainboard_init(device_t dev)
{
	configure_audio();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = "oak",
	.enable_dev = mainboard_enable,
};
