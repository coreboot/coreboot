/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <boardid.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
#include <gpio.h>
#include <soc/bl31_plat_params.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/grf.h>
#include <soc/i2c.h>
#include <soc/usb.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

/*
 * Wifi's PDN/RST line is pulled down by its (unpowered) voltage rails, but
 * this reset pin is pulled up by default. Let's drive it low as early as we
 * can.
 */
static void deassert_wifi_power(void)
{
	gpio_output(GPIO(1, B, 3), 0);  /* Assert WLAN_MODULE_RST# */
}

static void configure_emmc(void)
{
	/* Host controller does not support programmable clock generator.
	 * If we don't do this setting, when we use phy to control the
	 * emmc clock(when clock exceed 50MHz), it will get wrong clock.
	 *
	 * Refer to TRM V0.3 Part 1 Chapter 15 PAGE 782 for this register.
	 * Please search "_CON11[7:0]" to locate register description.
	 */
	write32(&rk3399_grf->emmccore_con[11], RK_CLRSETBITS(0xff, 0));

	rkclk_configure_emmc();
}

static void register_apio_suspend(void)
{
	static struct bl31_apio_param param_apio = {
		.h = {
			.type = PARAM_SUSPEND_APIO,
		},
		.apio = {
			.apio1 = 1,
			.apio2 = 1,
			.apio3 = 1,
			.apio4 = 1,
			.apio5 = 1,
		},
	};
	register_bl31_param(&param_apio.h);
}

static void register_gpio_suspend(void)
{
	/*
	 * these 3 pin to disable gpio2 ~ gpio4 1.5v, 1.8v, 3.3v power supply
	 * so need to shut down the power supply from high voltage to
	 * low voltage, and consider register_bl31() appends to the front off
	 * the list, we should register 1.5v enable pin to 3.3v enable pin
	 */
	static struct bl31_gpio_param param_p15_en = {
		.h = {
			.type = PARAM_SUSPEND_GPIO,
		},
		.gpio = {
			.polarity = BL31_GPIO_LEVEL_LOW,
		},
	};
	param_p15_en.gpio.index = GET_GPIO_NUM(GPIO_P15V_EN);
	register_bl31_param(&param_p15_en.h);

	static struct bl31_gpio_param param_p18_audio_en = {
		.h = {
			.type = PARAM_SUSPEND_GPIO,
		},
		.gpio = {
			.polarity = BL31_GPIO_LEVEL_LOW,
		},
	};
	param_p18_audio_en.gpio.index = GET_GPIO_NUM(GPIO_P18V_AUDIO_PWREN);
	register_bl31_param(&param_p18_audio_en.h);

	static struct bl31_gpio_param param_p30_en = {
		.h = {
			.type = PARAM_SUSPEND_GPIO,
		},
		.gpio = {
			.polarity = BL31_GPIO_LEVEL_LOW,
		},
	};
	param_p30_en.gpio.index = GET_GPIO_NUM(GPIO_P30V_EN);
	register_bl31_param(&param_p30_en.h);
}

static void register_reset_to_bl31(void)
{
	static struct bl31_gpio_param param_reset = {
		.h = {
			.type = PARAM_RESET,
		},
		.gpio = {
			.polarity = 1,
		},
	};

	/* gru/kevin reset pin: gpio0b3 */
	param_reset.gpio.index = GET_GPIO_NUM(GPIO_RESET),

	register_bl31_param(&param_reset.h);
}

static void register_poweroff_to_bl31(void)
{
	static struct bl31_gpio_param param_poweroff = {
		.h = {
			.type = PARAM_POWEROFF,
		},
		.gpio = {
			.polarity = 1,
		},
	};

	/*
	 * gru/kevin power off pin: gpio1a6,
	 * reuse with tsadc int pin, so iomux need set back to
	 * gpio in BL31 and depthcharge before you setting this gpio
	 */
	param_poweroff.gpio.index = GET_GPIO_NUM(GPIO_POWEROFF),

	register_bl31_param(&param_poweroff.h);
}

static void configure_sdmmc(void)
{
	gpio_output(GPIO(4, D, 5), 1);  /* SDMMC_PWR_EN */
	gpio_output(GPIO(2, A, 2), 1);  /* SDMMC_SDIO_PWR_EN */

	/* SDMMC_DET_L is different on Kevin board revision 0. */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN) && (board_id() == 0))
		gpio_input(GPIO(4, D, 2));
	else
		gpio_input(GPIO(4, D, 0));

	gpio_output(GPIO(2, D, 4), 0);  /* Keep the max voltage */

	gpio_input(GPIO(4, B, 0));	/* SDMMC0_D0 remove pull-up */
	gpio_input(GPIO(4, B, 1));	/* SDMMC0_D1 remove pull-up */
	gpio_input(GPIO(4, B, 2));	/* SDMMC0_D2 remove pull-up */
	gpio_input(GPIO(4, B, 3));	/* SDMMC0_D3 remove pull-up */
	gpio_input(GPIO(4, B, 4));	/* SDMMC0_CLK remove pull-down */
	gpio_input(GPIO(4, B, 5));	/* SDMMC0_CMD remove pull-up */

	write32(&rk3399_grf->gpio2_p[2][1], RK_CLRSETBITS(0xfff, 0));

	/*
	 * Set all outputs' drive strength to 8 mA. Group 4 bank B driver
	 * strength requires three bits per pin. Value of 2 written in that
	 * three bit field means '8 mA', as deduced from the kernel code.
	 *
	 * Thus the six pins involved in SDMMC interface require 18 bits to
	 * configure drive strength, but each 32 bit register provides only 16
	 * bits for this setting, this covers 5 pins fully and one bit from
	 * the 6th pin. Two more bits spill over to the next register. This is
	 * described on page 378 of rk3399 TRM Version 0.3 Part 1.
	 */
	write32(&rk3399_grf->gpio4b_e01,
		RK_CLRSETBITS(0xffff,
			      (2 << 0) | (2 << 3) |
			      (2 << 6) | (2 << 9) | (2 << 12)));
	write32(&rk3399_grf->gpio4b_e2, RK_CLRSETBITS(3, 1));

	/* And now set the multiplexor to enable SDMMC0. */
	write32(&rk3399_grf->iomux_sdmmc, IOMUX_SDMMC);
}

static void configure_codec(void)
{
	gpio_input(GPIO(3, D, 0));	/* I2S0_SCLK remove pull-up */
	gpio_input(GPIO(3, D, 1));	/* I2S0_RX remove pull-up */
	gpio_input(GPIO(3, D, 2));	/* I2S0_TX remove pull-up */
	gpio_input(GPIO(3, D, 3));	/* I2S0_SDI0 remove pull-up */
	gpio_input(GPIO(3, D, 4));	/* I2S0_SDI1 remove pull-up */
	/* GPIO3_D5 (I2S0_SDI2SDO2) not connected */
	gpio_input(GPIO(3, D, 6));	/* I2S0_SDO1 remove pull-up */
	gpio_input(GPIO(3, D, 7));	/* I2S0_SDO0 remove pull-up */
	gpio_input(GPIO(4, A, 0));	/* I2S0_MCLK remove pull-up */

	write32(&rk3399_grf->iomux_i2s0, IOMUX_I2S0);
	write32(&rk3399_grf->iomux_i2sclk, IOMUX_I2SCLK);

	/* AUDIO IO domain 1.8V voltage selection */
	write32(&rk3399_grf->io_vsel, RK_SETBITS(1 << 1));

	/* CPU1_P1.8V_AUDIO_PWREN for P1.8_AUDIO */
	gpio_output(GPIO(0, A, 2), 1);

	/* set CPU1_SPK_PA_EN output */
	gpio_output(GPIO(1, A, 2), 0);

	rkclk_configure_i2s(12288000);
}

static void configure_display(void)
{
	/* set pinmux for edp HPD*/
	gpio_input_pulldown(GPIO(4, C, 7));
	write32(&rk3399_grf->iomux_edp_hotplug, IOMUX_EDP_HOTPLUG);

	gpio_output(GPIO(4, D, 3), 1); /* CPU3_EDP_VDDEN for P3.3V_DISP */
}

static void setup_usb(void)
{
	/* A few magic PHY tuning values that improve eye diagram amplitude
	 * and make it extra sure we get reliable communication in firmware. */
	/* Set max ODT compensation voltage and current tuning reference. */
	write32(&rk3399_grf->usbphy0_ctrl[3], 0x0fff02e3);
	write32(&rk3399_grf->usbphy1_ctrl[3], 0x0fff02e3);
	/* Set max pre-emphasis level, only on Kevin PHY0 and PHY1,
	 * and disable the pre-emphasize in eop state to avoid
	 * mis-trigger the disconnect detection. */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN)) {
		write32(&rk3399_grf->usbphy0_ctrl[12], 0xffff00a7);
		write32(&rk3399_grf->usbphy1_ctrl[12], 0xffff00a7);
		write32(&rk3399_grf->usbphy0_ctrl[0], 0x00010000);
		write32(&rk3399_grf->usbphy1_ctrl[0], 0x00010000);
		write32(&rk3399_grf->usbphy0_ctrl[13], 0x00010000);
		write32(&rk3399_grf->usbphy1_ctrl[13], 0x00010000);
	}

	setup_usb_otg0();
	setup_usb_otg1();
}

static void mainboard_init(device_t dev)
{
	deassert_wifi_power();
	configure_sdmmc();
	configure_emmc();
	configure_codec();
	configure_display();
	setup_usb();
	register_reset_to_bl31();
	register_poweroff_to_bl31();
	register_gpio_suspend();
	register_apio_suspend();
}

static void enable_backlight_booster(void)
{
	const struct {
		uint8_t reg;
		uint8_t value;
	} i2c_writes[] = {
		{1, 0x84},
		{1, 0x85},
		{0, 0x26}
	};
	int i;
	const int booster_i2c_port = 0;
	uint8_t i2c_buf[2];
	struct i2c_seg i2c_command = { .read = 0, .chip = 0x2c,
				       .buf = i2c_buf, .len = sizeof(i2c_buf)
	};

	/*
	 * This function is called on Gru right after BL_EN is asserted. It
	 * takes time for the switcher chip to come online, let's wait a bit
	 * to let the voltage settle, so that the chip can be accessed.
	 */
	udelay(1000);

	gpio_input(GPIO(1, B, 7));	/* I2C0_SDA remove pull_up */
	gpio_input(GPIO(1, C, 0));	/* I2C0_SCL remove pull_up */

	i2c_init(0, 100*KHz);

	write32(&rk3399_pmugrf->iomux_i2c0_sda, IOMUX_I2C0_SDA);
	write32(&rk3399_pmugrf->iomux_i2c0_scl, IOMUX_I2C0_SCL);

	for (i = 0; i < ARRAY_SIZE(i2c_writes); i++) {
		i2c_buf[0] = i2c_writes[i].reg;
		i2c_buf[1] = i2c_writes[i].value;
		i2c_transfer(booster_i2c_port, &i2c_command, 1);
	}
}

void mainboard_power_on_backlight(void)
{
	gpio_output(GPIO_BACKLIGHT, 1);  /* BL_EN */

	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_GRU) && board_id() == 0)
		enable_backlight_booster();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
