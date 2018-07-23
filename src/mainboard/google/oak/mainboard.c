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
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <drivers/parade/ps8640/ps8640.h>
#include <edid.h>

#include <elog.h>
#include <gpio.h>
#include <soc/da9212.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/i2c.h>
#include <soc/mt6311.h>
#include <soc/mt6391.h>
#include <soc/mtcmos.h>
#include <soc/pll.h>
#include <soc/usb.h>
#include <vendorcode/google/chromeos/chromeos.h>

enum {
	CODEC_I2C_BUS = 0,
	EXT_BUCK_I2C_BUS = 1,
};

static void configure_ext_buck(void)
{
	mtk_i2c_bus_init(EXT_BUCK_I2C_BUS);

	switch (board_id() + CONFIG_BOARD_ID_ADJUSTMENT) {
	case 3:
	case 4:
		/* rev-3 and rev-4 use mt6311 as external buck */
		gpio_output(GPIO(EINT15), 1);
		udelay(500);
		mt6311_probe(EXT_BUCK_I2C_BUS);
		break;
	case 2:
	default:
		/* rev-2 and rev-5 use da9212 as external buck */
		mt6391_gpio_output(MT6391_KP_ROW3, 1); /* DA9212_IC_EN */
		mt6391_gpio_output(MT6391_KP_ROW4, 1); /* DA9212_EN_A */
		udelay(500);	/* add 500us delay for powering on da9212 */
		da9212_probe(EXT_BUCK_I2C_BUS);
		break;
	}
}

static void configure_touchscreen(void)
{
	/* Pull low reset gpio for 500us and then pull high */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT >= 7) {
		gpio_output(GPIO(PCM_SYNC), 0);
		udelay(500);
		gpio_output(GPIO(PCM_SYNC), 1);
	}
}

static void configure_audio(void)
{
	mtcmos_audio_power_on();

	/* vgp1 set to 1.8V */
	mt6391_configure_ldo(LDO_VCAMD, LDO_1P8);
	/* delay 1ms for realtek's power sequence request */
	mdelay(1);
	/* vcama set to 1.8V */
	mt6391_configure_ldo(LDO_VCAMA, LDO_1P8);

	/* reset ALC5676 */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		gpio_output(GPIO(LCM_RST), 1);

	/* SoC I2S */
	gpio_set_mode(GPIO(I2S0_LRCK), PAD_I2S0_LRCK_FUNC_I2S1_WS);
	gpio_set_mode(GPIO(I2S0_BCK), PAD_I2S0_BCK_FUNC_I2S1_BCK);
	gpio_set_mode(GPIO(I2S0_MCK), PAD_I2S0_MCK_FUNC_I2S1_MCK);
	gpio_set_mode(GPIO(I2S0_DATA0), PAD_I2S0_DATA0_FUNC_I2S1_DO_1);
	gpio_set_mode(GPIO(I2S0_DATA1), PAD_I2S0_DATA1_FUNC_I2S2_DI_2);
	/* codec ext MCLK ON */
	mt6391_gpio_output(MT6391_KP_COL4, 1);

	switch (board_id() + CONFIG_BOARD_ID_ADJUSTMENT) {
	case 2:
	case 3:
	case 4:
		mt6391_gpio_output(MT6391_KP_COL5, 1);
		break;
	case 5:
	case 6:
		gpio_set_mode(GPIO(UCTS0), PAD_UCTS0_FUNC_I2S2_DI_1);
		mt6391_gpio_output(MT6391_KP_COL5, 1);
		break;
	default:
		break;
	}

	/* Init i2c bus Timing register for audio codecs */
	mtk_i2c_bus_init(CODEC_I2C_BUS);

	/* set I2S clock to 48KHz */
	mt_pll_set_aud_div(48 * KHz);
}

static void configure_usb(void)
{
	setup_usb_host();

	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT > 3) {
		/* Type C port 0 Over current alert pin */
		gpio_input_pullup(GPIO(MSDC3_DSL));
		if (!IS_ENABLED(CONFIG_BOARD_GOOGLE_ROWAN)) {
			/* Enable USB3 type A port 0 5V load switch */
			gpio_output(GPIO(CM2MCLK), 1);
			/* USB3 Type A port 0 power over current alert pin */
			gpio_input_pullup(GPIO(CMPCLK));
		}
		/* Type C port 1 over current alert pin */
		if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 7)
			gpio_input_pullup(GPIO(PCM_SYNC));
	}

	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT > 4 &&
	    board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 7)
	{
		/* USB 2.0 type A port over current interrupt pin(low active) */
		gpio_input_pullup(GPIO(UCTS2));
		/* USB 2.0 type A port BC1.2 STATUS(low active) */
		gpio_input_pullup(GPIO(AUD_DAT_MISO));
	}
}

static void configure_usb_hub(void)
{
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_ROWAN))
		return;

	/* set usb hub reset pin (low active) to high */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT > 4)
		gpio_output(GPIO(UTXD3), 1);
}

/* Setup backlight control pins as output pin and power-off by default */
static void configure_backlight(void)
{
	/* Configure PANEL_LCD_POWER_EN */
	switch (board_id() + CONFIG_BOARD_ID_ADJUSTMENT) {
	case 3:
		gpio_output(GPIO(UCTS2), 0);
		break;
	case 4:
		gpio_output(GPIO(SRCLKENAI), 0);
		break;
	default:
		gpio_output(GPIO(UTXD2), 0);
		break;
	}

	gpio_output(GPIO(DISP_PWM0), 0);	/* DISP_PWM0 */
	gpio_output(GPIO(PCM_TX), 0);	/* PANEL_POWER_EN */
}

static void configure_display(void)
{
	/* board from Rev2 */
	gpio_output(GPIO(CMMCLK), 1); /* PANEL_3V3_ENABLE */
	/* vgp2 set to 3.3V for ps8640 */
	mt6391_configure_ldo(LDO_VGP2, LDO_3P3);
	gpio_output(GPIO(URTS0), 0); /* PS8640_SYSRSTN */
	/* PS8640_1V2_ENABLE */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT == 4)
		gpio_output(GPIO(SRCLKENAI2), 1);
	else
		gpio_output(GPIO(URTS2), 1);
	/* delay 2ms for vgp2 and PS8640_1V2_ENABLE stable */
	mdelay(2);
	/* PS8640_PDN */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT > 4)
		gpio_output(GPIO(LCM_RST), 1);
	else
		gpio_output(GPIO(UCTS0), 1);
	gpio_output(GPIO(PCM_CLK), 1); /* PS8640_MODE_CONF */
	gpio_output(GPIO(URTS0), 1); /* PS8640_SYSRSTN */
	/* for level shift(1.8V to 3.3V) on */
	udelay(100);
}

static void configure_backlight_rowan(void)
{
	gpio_output(GPIO(DAIPCMOUT), 0);	/* PANEL_LCD_POWER_EN */
	gpio_output(GPIO(DISP_PWM0), 0);	/* DISP_PWM0 */
	gpio_output(GPIO(PCM_TX), 0);	/* PANEL_POWER_EN */
}

static void configure_display_rowan(void)
{
	gpio_output(GPIO(UCTS2), 1); /* VDDIO_EN */
	/* delay 15 ms for panel vddio to stabilize */
	mdelay(15);

	gpio_output(GPIO(SRCLKENAI2), 1); /* LCD_RESET */
	udelay(20);
	gpio_output(GPIO(SRCLKENAI2), 0); /* LCD_RESET */
	udelay(20);
	gpio_output(GPIO(SRCLKENAI2), 1); /* LCD_RESET */
	mdelay(20);

	/* Rowan panel avdd */
	gpio_output(GPIO(URTS2), 1);

	/* Rowan panel avee */
	gpio_output(GPIO(URTS0), 1);

	/* panel.delay.prepare */
	mdelay(20);
}

static const struct edid rowan_boe_edid = {
	.panel_bits_per_color = 8,
	.panel_bits_per_pixel = 24,
	.mode = {
		.name = "1536x2048@60Hz",
		.pixel_clock = 241646,
		.lvds_dual_channel = 1,
		.refresh = 60,
		.ha = 1536, .hbl = 404, .hso = 200, .hspw = 4, .hborder = 0,
		.va = 2048, .vbl = 28, .vso = 12, .vspw = 2, .vborder = 0,
		.phsync = '-', .pvsync = '-',
		.x_mm = 147, .y_mm = 196,
	},
};

static int read_edid_from_ps8640(struct edid *edid)
{
	u8 i2c_bus, i2c_addr;

	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT > 6) {
		i2c_bus = 0;
		i2c_addr = 0x8;
	} else {
		i2c_bus = 4;
		i2c_addr = 0x18;
	}

	mtk_i2c_bus_init(i2c_bus);

	ps8640_init(i2c_bus, i2c_addr);
	if (ps8640_get_edid(i2c_bus, i2c_addr, edid)) {
		printk(BIOS_ERR, "Can't get panel's edid\n");
		return -1;
	}

	return 0;
}

static void display_startup(void)
{
	struct edid edid;
	int ret;
	u32 mipi_dsi_flags;
	bool dual_dsi_mode;

	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_ROWAN)) {
		edid = rowan_boe_edid;
		dual_dsi_mode = true;
		mipi_dsi_flags = MIPI_DSI_MODE_VIDEO |
				 MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
				 MIPI_DSI_MODE_LPM | MIPI_DSI_MODE_EOT_PACKET |
				 MIPI_DSI_CLOCK_NON_CONTINUOUS;
	} else {
		if (read_edid_from_ps8640(&edid) < 0)
			return;

		dual_dsi_mode = false;
		mipi_dsi_flags = MIPI_DSI_MODE_VIDEO |
				 MIPI_DSI_MODE_VIDEO_SYNC_PULSE;
	}

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	mtk_ddp_init(dual_dsi_mode);
	ret = mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4,
			   dual_dsi_mode, &edid);
	if (ret < 0) {
		printk(BIOS_ERR, "dsi init fail\n");
		return;
	}

	mtk_ddp_mode_set(&edid, dual_dsi_mode);

	set_vbe_mode_info_valid(&edid, (uintptr_t)0);
}

static void mainboard_init(struct device *dev)
{
	/* TP_SHIFT_EN: Enables the level shifter for I2C bus 4 (TPAD), which
	 * also contains the PS8640 eDP brige and the USB hub.
	 */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		mt6391_gpio_output(MT6391_KP_ROW2, 1);

	/* Config SD card detection pin */
	gpio_input_pullup(GPIO(EINT1)); /* SD_DET */

	configure_audio();

	/* fix dsi lp mode is half voltage attenuation */
	mtk_dsi_pin_drv_ctrl();

	if (display_init_required()) {
		mtcmos_display_power_on();
		if (IS_ENABLED(CONFIG_BOARD_GOOGLE_ROWAN)) {
			configure_backlight_rowan();
			configure_display_rowan();
		} else {
			configure_backlight();
			configure_display();
		}
		display_startup();
	} else {
		printk(BIOS_INFO, "Skipping display init.\n");
	}
	configure_usb();
	configure_usb_hub();
	configure_ext_buck();
	configure_touchscreen();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = "oak",
	.enable_dev = mainboard_enable,
};
