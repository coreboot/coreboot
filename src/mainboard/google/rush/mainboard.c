/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/mmu.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <delay.h>
#include <device/device.h>
#include <memrange.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra/usb.h>
#include <soc/padconfig.h>
#include <soc/spi.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/display.h>

static const struct pad_config sdmmc3_pad[] = {
	/* MMC3(SDCARD) */
	PAD_CFG_SFIO(SDMMC3_CLK, PINMUX_INPUT_ENABLE, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_CMD, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT0, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT2, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_DAT3, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_CLK_LB_IN, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC3),
	PAD_CFG_SFIO(SDMMC3_CLK_LB_OUT, PINMUX_INPUT_ENABLE | PINMUX_PULL_DOWN, SDMMC3),
	/* MMC3 Card Detect pin */
	PAD_CFG_GPIO_INPUT(SDMMC3_CD_N, PINMUX_PULL_UP),
	/* Disable SD card reader power so it can be reset even on warm boot.
	   Payloads must enable power before accessing SD card slots. */
	PAD_CFG_GPIO_OUT0(KB_ROW0, PINMUX_PULL_NONE),
};

static const struct pad_config sdmmc4_pad[] = {
	/* MMC4 (eMMC) */
	PAD_CFG_SFIO(SDMMC4_CLK, PINMUX_INPUT_ENABLE, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_CMD, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT0, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT2, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT3, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT4, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT5, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT6, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT7, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC4),
};

static const struct pad_config padcfgs[] = {
	/* We pull the USB VBUS signals up but keep them as inputs since the
	 * voltage source likes to drive them low on overcurrent conditions */
	PAD_CFG_GPIO_INPUT(USB_VBUS_EN0, PINMUX_PULL_UP),
	PAD_CFG_GPIO_INPUT(USB_VBUS_EN1, PINMUX_PULL_UP),

	/* backlight_vdd_gpio: P2 */
	PAD_CFG_GPIO_OUT0(DAP3_DOUT, PINMUX_PULL_NONE),

	/* backlight_en_gpio: H2 */
	PAD_CFG_GPIO_OUT0(GPIO_PH2, PINMUX_PULL_NONE),

	/* backlight_pwm: H1 */
	PAD_CFG_SFIO(GPIO_PH1, PINMUX_PULL_NONE, PWM1),

	/* DP HPD */
	PAD_CFG_SFIO(DP_HPD, PINMUX_INPUT_ENABLE, DP),
};

static const struct pad_config i2c1_pad[] = {
	/* GEN1 I2C */
	PAD_CFG_SFIO(GEN1_I2C_SCL, PINMUX_INPUT_ENABLE, I2C1),
	PAD_CFG_SFIO(GEN1_I2C_SDA, PINMUX_INPUT_ENABLE, I2C1),
};

static const struct funit_cfg funitcfgs[] = {
	FUNIT_CFG(SDMMC3, PLLP, 48000, sdmmc3_pad, ARRAY_SIZE(sdmmc3_pad)),
	FUNIT_CFG(SDMMC4, PLLP, 48000, sdmmc4_pad, ARRAY_SIZE(sdmmc4_pad)),
	FUNIT_CFG(I2C1, PLLP, 100, i2c1_pad, ARRAY_SIZE(i2c1_pad)),
};

static void setup_ec_spi(void)
{
	struct tegra_spi_channel *spi;

	spi = tegra_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);
}

static void setup_usb(void)
{
	clock_enable_clear_reset(CLK_L_USBD, CLK_H_USB3, 0, 0, 0, 0);

	usb_setup_utmip((void *)TEGRA_USBD_BASE);
	usb_setup_utmip((void *)TEGRA_USB3_BASE);
}

static const struct pad_config i2s1_pad[] = {
	/* I2S1 */
	PAD_CFG_SFIO(DAP2_SCLK, PINMUX_INPUT_ENABLE, I2S1),
	PAD_CFG_SFIO(DAP2_FS, PINMUX_INPUT_ENABLE, I2S1),
	PAD_CFG_SFIO(DAP2_DOUT, PINMUX_INPUT_ENABLE, I2S1),
	PAD_CFG_SFIO(DAP2_DIN, PINMUX_INPUT_ENABLE | PINMUX_TRISTATE, I2S1),
	/* codec MCLK via EXTPERIPH1 */
	PAD_CFG_SFIO(DAP_MCLK1, PINMUX_PULL_NONE, EXTPERIPH1),
};

static const struct funit_cfg audio_funit[] = {
	/* We need 1.5MHz for I2S1. So we use CLK_M */
	FUNIT_CFG(I2S1, CLK_M, 1500, i2s1_pad, ARRAY_SIZE(i2s1_pad)),
};

static void configure_display_clocks(void)
{
	u32 lclks = CLK_L_DISP1 | CLK_L_HOST1X | CLK_L_PWM;
	u32 xclks = CLK_X_DPAUX | CLK_X_SOR0;

	clock_enable_clear_reset(lclks, 0, 0, 0, 0, xclks);

	/* Give clocks time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);
}

static int configure_display_blocks(void)
{
	soc_configure_host1x();

	/* enable display related clocks */
	configure_display_clocks();

	return 0;
}

/* Audio init: clocks and enables/resets */
static void setup_audio(void)
{
	/*
	 * External peripheral 1: audio codec (max98090) uses 12MHz CLK1
	 * NOTE: We can't use a funits struct/call here because EXTPERIPH1/2/3
	 * don't have BASE regs or CAR RST/ENA bits. Also, the mux setting for
	 * EXTPERIPH1/DAP_MCLK1 is rolled into the I2S1 padcfg.
	 */
	clock_configure_source(extperiph1, CLK_M, 12000);

	soc_configure_funits(audio_funit, ARRAY_SIZE(audio_funit));

	clock_external_output(1);	/* For external MAX98090 audio codec. */

	/*
	 * Confirmed by NVIDIA hardware team, we need to take ALL audio devices
	 * connected to AHUB (AUDIO, APBIF, I2S, DAM, AMX, ADX, SPDIF, AFC) out
	 * of reset and clock-enabled, otherwise reading AHUB devices (in our
	 * case, I2S/APBIF/AUDIO<XBAR>) will hang.
	 */
	clock_enable_audio();
}

static void mainboard_init(device_t dev)
{
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));
	soc_configure_funits(funitcfgs, ARRAY_SIZE(funitcfgs));

	setup_ec_spi();
	setup_usb();

	setup_audio();
	i2c_init(I2C1_BUS);		/* for max98090 codec */

	/* if panel needs to bringup */
	if (display_init_required())
		configure_display_blocks();
}

void display_startup(device_t dev)
{
	dp_display_startup(dev);
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name   = "rush",
	.enable_dev = mainboard_enable,
};
