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

#include <elog.h>
#include <gpio.h>
#include <soc/bl31_plat_params.h>
#include <soc/i2c.h>
#include <soc/mt6391.h>
#include <soc/mtcmos.h>
#include <soc/pinmux.h>
#include <soc/pll.h>
#include <soc/usb.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void register_da9212_to_bl31(void)
{
#if IS_ENABLED(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE)
	static struct bl31_da9212_param param_da9212 = {
		.h = {
			.type = PARAM_CLUSTER1_DA9212,
		},
		.i2c_bus = 1,
		.ic_en = {
			.type = PARAM_GPIO_MT6391,
			.polarity = PARAM_GPIO_ACTIVE_HIGH,
			.index = MT6391_KP_ROW3,
		},
		.en_a = {
			.type = PARAM_GPIO_MT6391,
			.polarity = PARAM_GPIO_ACTIVE_HIGH,
			.index = MT6391_KP_ROW4,
		},
		.en_b = {
			.type = PARAM_GPIO_NONE,
		},
	};
	if (board_id() == 2) {
		param_da9212.ic_en.type = PARAM_GPIO_SOC;
		param_da9212.ic_en.index = PAD_UCTS2;
	}
	register_bl31_param(&param_da9212.h);

	/* Init i2c bus Timing register for da9212 */
	mtk_i2c_bus_init(param_da9212.i2c_bus);
#endif
}

static void register_mt6311_to_bl31(void)
{
#if IS_ENABLED(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE)
	static struct bl31_mt6311_param param_mt6311 = {
		.h = {
			.type = PARAM_CLUSTER1_MT6311,
		},
		.i2c_bus = 1,
	};
	register_bl31_param(&param_mt6311.h);

	/* Init i2c bus Timing register for mt6311 */
	mtk_i2c_bus_init(param_mt6311.i2c_bus);
#endif
}

static void configure_bl31(void)
{
	switch (board_id()) {
	case 3:
	case 4:
		/* rev-3 and rev-4 use mt6311 as external buck */
		register_mt6311_to_bl31();
		break;
	case 2:
	default:
		/* rev-2 and rev-5 use da9212 as external buck */
		register_da9212_to_bl31();
		break;
	}
}

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

	/* Init i2c bus Timing register for audio codecs */
	mtk_i2c_bus_init(0);

	/* set I2S clock to 48KHz */
	mt_pll_set_aud_div(48 * KHz);
}

static void configure_usb(void)
{
	setup_usb_host();

	if (board_id() > 3)
		gpio_output(PAD_CM2MCLK, 1);
}

/* Setup backlight control pins as output pin and power-off by default */
static void configure_backlight(void)
{
	/* Configure PANEL_LCD_POWER_EN */
	switch (board_id()) {
	case 1:
	case 2:
		break;
	case 3:
		gpio_output(PAD_UCTS2, 0);
		break;
	case 4:
		gpio_output(PAD_SRCLKENAI, 0);
		break;
	default:
		gpio_output(PAD_UTXD2, 0);
		break;
	}

	gpio_output(PAD_DISP_PWM0, 0);	/* DISP_PWM0 */
	gpio_output(PAD_PCM_TX, 0);	/* PANEL_POWER_EN */
}

static void mainboard_init(device_t dev)
{
	/* TP_SHIFT_EN: Enables the level shifter for I2C bus 4 (TPAD), which
	 * also contains the PS8640 eDP brige and the USB hub.
	 */
	if (board_id() < 5)
		mt6391_gpio_output(MT6391_KP_ROW2, 1);

	/* Config SD card detection pin */
	gpio_input(PAD_EINT1); /* SD_DET */

	configure_audio();
	configure_backlight();
	configure_usb();
	configure_bl31();

	elog_init();
	elog_add_boot_reason();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = "oak",
	.enable_dev = mainboard_enable,
};
