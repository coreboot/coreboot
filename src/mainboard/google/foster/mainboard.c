/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 * Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
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

#include <arch/io.h>
#include <arch/mmu.h>
#include <boot/coreboot_tables.h>
#include <delay.h>
#include <device/device.h>
#include <memrange.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/padconfig.h>
#include <soc/spi.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/display.h>

#include <vendorcode/google/chromeos/chromeos.h>

static const struct pad_config sdmmc1_pad[] = {
	/* MMC1(SDCARD) */
	PAD_CFG_SFIO(SDMMC1_CLK, PINMUX_INPUT_ENABLE, SDMMC1),
	PAD_CFG_SFIO(SDMMC1_CMD, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC1),
	PAD_CFG_SFIO(SDMMC1_DAT0, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC1),
	PAD_CFG_SFIO(SDMMC1_DAT1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC1),
	PAD_CFG_SFIO(SDMMC1_DAT2, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC1),
	PAD_CFG_SFIO(SDMMC1_DAT3, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, SDMMC1),
	/* MMC1 Card Detect pin */
	PAD_CFG_GPIO_INPUT(GPIO_PZ1, PINMUX_PULL_UP),
	/* Disable SD card reader power so it can be reset even on warm boot.
	   Payloads must enable power before accessing SD card slots. */
	PAD_CFG_GPIO_OUT0(GPIO_PZ4, PINMUX_PULL_NONE),
};

static const struct pad_config audio_codec_pads[] = {
	/* GPIO_X1_AUD(BB3) is AUDIO_LDO_EN (->CODEC RESET_N pin) */
	PAD_CFG_GPIO_OUT1(GPIO_X1_AUD, PINMUX_PULL_DOWN),
};

static const struct pad_config padcfgs[] = {
	/* We pull the USB VBUS signals up but keep them as inputs since the
	 * voltage source likes to drive them low on overcurrent conditions */
	PAD_CFG_GPIO_INPUT(USB_VBUS_EN1, PINMUX_PULL_NONE | PINMUX_PARKED |
			   PINMUX_INPUT_ENABLE | PINMUX_LPDR | PINMUX_IO_HV),

	/* Add backlight vdd/enable/pwm/dp hpd pad cfgs here */
};

static const struct pad_config i2c1_pad[] = {
	/* GEN1 I2C */
	PAD_CFG_SFIO(GEN1_I2C_SCL, PINMUX_INPUT_ENABLE, I2C1),
	PAD_CFG_SFIO(GEN1_I2C_SDA, PINMUX_INPUT_ENABLE, I2C1),
};

static const struct pad_config i2s1_pad[] = {
	/* I2S1 */
	PAD_CFG_SFIO(DAP1_SCLK, PINMUX_INPUT_ENABLE, I2S1),
	PAD_CFG_SFIO(DAP1_FS, PINMUX_INPUT_ENABLE, I2S1),
	PAD_CFG_SFIO(DAP1_DOUT, PINMUX_INPUT_ENABLE, I2S1),
	PAD_CFG_SFIO(DAP1_DIN, PINMUX_INPUT_ENABLE | PINMUX_TRISTATE, I2S1),
	/* codec MCLK via AUD SFIO */
	PAD_CFG_SFIO(AUD_MCLK, PINMUX_PULL_NONE, AUD),
};

static const struct funit_cfg audio_funit[] = {
	/* We need 1.5MHz for I2S1. So we use CLK_M */
	FUNIT_CFG(I2S1, CLK_M, 1500, i2s1_pad, ARRAY_SIZE(i2s1_pad)),
};

static const struct funit_cfg funitcfgs[] = {
	FUNIT_CFG(SDMMC1, PLLP, 48000, sdmmc1_pad, ARRAY_SIZE(sdmmc1_pad)),
	FUNIT_CFG(SDMMC4, PLLP, 48000, NULL, 0),
	FUNIT_CFG(I2C1, PLLP, 100, i2c1_pad, ARRAY_SIZE(i2c1_pad)),
	FUNIT_CFG(I2C6, PLLP, 400, audio_codec_pads, ARRAY_SIZE(audio_codec_pads)),
	FUNIT_CFG_USB(USBD),
};

/* Audio init: clocks and enables/resets */
static void setup_audio(void)
{
	/* Audio codec (ES755) uses OSC freq (via AUD_MCLK), s/b 38.4MHz */
	soc_configure_funits(audio_funit, ARRAY_SIZE(audio_funit));

	/*
	 * As per NVIDIA hardware team, we need to take ALL audio devices
	 * connected to AHUB (AUDIO, APB2APE, I2S, SPDIF, etc.) out of reset
	 * and clock-enabled, otherwise reading AHUB devices (in our case,
	 * I2S/APBIF/AUDIO<XBAR>) will hang.
	 */
	soc_configure_ape();
	clock_enable_audio();
}

static void mainboard_init(struct device *dev)
{
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));
	soc_configure_funits(funitcfgs, ARRAY_SIZE(funitcfgs));

	i2c_init(I2C1_BUS);
	setup_audio();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name   = "foster",
	.enable_dev = mainboard_enable,
};
