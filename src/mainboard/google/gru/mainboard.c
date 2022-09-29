/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bl31.h>
#include <boardid.h>
#include <bootmode.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/grf.h>
#include <soc/mipi.h>
#include <soc/i2c.h>
#include <soc/usb.h>
#include <string.h>

#include "board.h"

#include <arm-trusted-firmware/include/export/plat/rockchip/common/plat_params_exp.h>

/*
 * We have to drive the stronger pull-up within 1 second of powering up the
 * touchpad to prevent its firmware from falling into recovery. Not on
 * Scarlet-based boards.
 */
static void configure_touchpad(void)
{
	gpio_output(GPIO_TP_RST_L, 1); /* TP's I2C pull-up rail */
}

/*
 * Wifi's PDN/RST line is pulled down by its (unpowered) voltage rails, but
 * this reset pin is pulled up by default. Let's drive it low as early as we
 * can. This only applies to boards with Marvell 8997 WiFi.
 */
static void assert_wifi_reset(void)
{
	gpio_output(GPIO_WLAN_RST_L, 0);  /* Assert WLAN_MODULE_RST# */
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
	static struct bl_aux_param_rk_apio param_apio = {
		.h = {
			.type = BL_AUX_PARAM_RK_SUSPEND_APIO,
		},
		.apio = {
			.apio1 = 1,
			.apio2 = 1,
			.apio3 = 1,
			.apio4 = 1,
			.apio5 = 1,
		},
	};
	register_bl31_aux_param(&param_apio.h);
}

static void register_gpio_suspend(void)
{
	/*
	 * These three GPIO params are used to shut down the 1.5V, 1.8V and
	 * 3.3V power rails, which need to be shut down ordered by voltage,
	 * with highest voltage first.
	 * Since register_bl31() appends to the front of the list, we need to
	 * register them backwards, with 1.5V coming first.
	 * 1.5V and 1.8V are EC-controlled on Scarlet derivatives,
	 * so we skip them.
	 */
	if (!CONFIG(GRU_BASEBOARD_SCARLET)) {
		static struct bl_aux_param_gpio param_p15_en = {
			.h = { .type = BL_AUX_PARAM_RK_SUSPEND_GPIO },
			.gpio = { .polarity = ARM_TF_GPIO_LEVEL_LOW },
		};
		param_p15_en.gpio.index = GPIO_P15V_EN.raw;
		register_bl31_aux_param(&param_p15_en.h);

		static struct bl_aux_param_gpio param_p18_audio_en = {
			.h = { .type = BL_AUX_PARAM_RK_SUSPEND_GPIO },
			.gpio = { .polarity = ARM_TF_GPIO_LEVEL_LOW },
		};
		param_p18_audio_en.gpio.index = GPIO_P18V_AUDIO_PWREN.raw;
		register_bl31_aux_param(&param_p18_audio_en.h);
	}

	static struct bl_aux_param_gpio param_p30_en = {
		.h = { .type = BL_AUX_PARAM_RK_SUSPEND_GPIO },
		.gpio = { .polarity = ARM_TF_GPIO_LEVEL_LOW },
	};
	param_p30_en.gpio.index = GPIO_P30V_EN.raw;
	register_bl31_aux_param(&param_p30_en.h);
}

static void register_reset_to_bl31(void)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = {
			.type = BL_AUX_PARAM_RK_RESET_GPIO,
		},
		.gpio = {
			.polarity = 1,
		},
	};

	/* gru/kevin reset pin: gpio0b3 */
	param_reset.gpio.index = GPIO_RESET.raw;

	register_bl31_aux_param(&param_reset.h);
}

static void register_poweroff_to_bl31(void)
{
	static struct bl_aux_param_gpio param_poweroff = {
		.h = {
			.type = BL_AUX_PARAM_RK_POWEROFF_GPIO,
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
	param_poweroff.gpio.index = GPIO_POWEROFF.raw;

	register_bl31_aux_param(&param_poweroff.h);
}

static void configure_sdmmc(void)
{
	gpio_output(GPIO(2, A, 2), 1);  /* SDMMC_SDIO_PWR_EN */

	/* set SDMMC_DET_L pin */
	if (CONFIG(GRU_BASEBOARD_SCARLET))
		/*
		 * do not have external pull up, so need to
		 * set this pin internal pull up
		 */
		gpio_input_pullup(GPIO(1, B, 3));
	else
		gpio_input(GPIO(4, D, 0));

	/*
	 * Keep sd card io domain 3v
	 * In Scarlet derivatives, this GPIO set to high will get 3v,
	 * With other board variants setting this GPIO low results in 3V.
	 */
	if (CONFIG(GRU_BASEBOARD_SCARLET))
		gpio_output(GPIO(2, D, 4), 1);
	else
		gpio_output(GPIO(2, D, 4), 0);

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
	/* GPIOs 3_D4 - 3_D6 not used for I2S and are SKU ID pins on Scarlet. */
	gpio_input(GPIO(3, D, 7));	/* I2S0_SDO0 remove pull-up */
	gpio_input(GPIO(4, A, 0));	/* I2S0_MCLK remove pull-up */

	write32(&rk3399_grf->iomux_i2s0, IOMUX_I2S0_SD0);
	write32(&rk3399_grf->iomux_i2sclk, IOMUX_I2SCLK);

	if (!CONFIG(GRU_BASEBOARD_SCARLET))
		gpio_output(GPIO_P18V_AUDIO_PWREN, 1);
	gpio_output(GPIO_SPK_PA_EN, 0);

	rkclk_configure_i2s(12288000);
}

static void configure_display(void)
{
	/*
	 * Rainier is Scarlet-derived, but uses EDP so use board-specific
	 * config rather than baseboard.
	 */
	if (CONFIG(BOARD_GOOGLE_SCARLET)) {
		gpio_output(GPIO(4, D, 1), 0);	/* DISPLAY_RST_L */
		gpio_output(GPIO(4, D, 3), 1);	/* PPVARP_LCD */
		mdelay(10);
		gpio_output(GPIO(4, D, 4), 1);	/* PPVARN_LCD */
		mdelay(20 + 2);	/* add 2ms for bias rise time */
		gpio_output(GPIO(4, D, 1), 1);	/* DISPLAY_RST_L */
		mdelay(30);
	} else {
		/* set pinmux for edp HPD */
		gpio_input_pulldown(GPIO(4, C, 7));
		write32(&rk3399_grf->iomux_edp_hotplug, IOMUX_EDP_HOTPLUG);

		gpio_output(GPIO(4, D, 3), 1);	/* P3.3V_DISP */
	}
}

static void usb_power_cycle(int port)
{
	if (google_chromeec_set_usb_pd_role(port, USB_PD_CTRL_ROLE_FORCE_SINK))
		printk(BIOS_ERR, "Cannot force USB%d PD sink\n", port);

	mdelay(10);	/* Make sure USB stick is fully depowered. */

	if (google_chromeec_set_usb_pd_role(port, USB_PD_CTRL_ROLE_TOGGLE_ON))
		printk(BIOS_ERR, "Cannot restore USB%d PD mode\n", port);
}

static void setup_usb(int port)
{
	/* Must be PHY0 or PHY1. */
	assert(port == 0 || port == 1);

	/*
	 * A few magic PHY tuning values that improve eye diagram amplitude
	 * and make it extra sure we get reliable communication in firmware
	 * Set max ODT compensation voltage and current tuning reference.
	 */
	write32(&rk3399_grf->usbphy_ctrl[port][3], RK_CLRSETBITS(0xfff, 0x2e3));

	/* Set max pre-emphasis level on PHY0 and PHY1. */
	write32(&rk3399_grf->usbphy_ctrl[port][12],
		RK_CLRSETBITS(0xffff, 0xa7));

	/*
	 * 1. Disable the pre-emphasize in eop state and chirp
	 * state to avoid mis-trigger the disconnect detection
	 * and also avoid high-speed handshake fail for PHY0
	 * and PHY1 consist of otg-port and host-port.
	 *
	 * 2. Configure PHY0 and PHY1 otg-ports squelch detection
	 * threshold to 125mV (default is 150mV).
	 */
	write32(&rk3399_grf->usbphy_ctrl[port][0],
		RK_CLRSETBITS(7 << 13 | 3 << 0, 6 << 13));
	write32(&rk3399_grf->usbphy_ctrl[port][13], RK_CLRBITS(3 << 0));

	/*
	 * ODT auto compensation bypass, and set max driver
	 * strength only for PHY0 and PHY1 otg-port.
	 */
	write32(&rk3399_grf->usbphy_ctrl[port][2],
		RK_CLRSETBITS(0x7e << 4, 0x60 << 4));

	/*
	 * ODT auto refresh bypass, and set the max bias current
	 * tuning reference only for PHY0 and PHY1 otg-port.
	 */
	write32(&rk3399_grf->usbphy_ctrl[port][3],
		RK_CLRSETBITS(0x21c, 1 << 4));

	/*
	 * ODT auto compensation bypass, and set default driver
	 * strength only for PHY0 and PHY1 host-port.
	 */
	write32(&rk3399_grf->usbphy_ctrl[port][15], RK_SETBITS(1 << 10));

	/* ODT auto refresh bypass only for PHY0 and PHY1 host-port. */
	write32(&rk3399_grf->usbphy_ctrl[port][16], RK_CLRBITS(1 << 9));

	if (port == 0)
		setup_usb_otg0();
	else
		setup_usb_otg1();

	/*
	 * Need to power-cycle USB ports for use in firmware, since some devices
	 * can't fall back to USB 2.0 after they saw SuperSpeed terminations.
	 * This takes about a dozen milliseconds, so only do it in boot modes
	 * that have firmware UI (which one could select USB boot from).
	 */
	if (display_init_required())
		usb_power_cycle(port);
}

static void mainboard_init(struct device *dev)
{
	configure_sdmmc();
	configure_emmc();
	configure_codec();
	if (display_init_required())
		configure_display();
	setup_usb(0);
	if (CONFIG(GRU_HAS_WLAN_RESET))
		assert_wifi_reset();
	if (!CONFIG(GRU_BASEBOARD_SCARLET)) {
		configure_touchpad();		/* Scarlet: works differently */
		setup_usb(1);			/* Scarlet: only one USB port */
	}
	register_gpio_suspend();
	register_reset_to_bl31();
	register_poweroff_to_bl31();
	register_apio_suspend();
}

static void prepare_backlight_i2c(void)
{
	gpio_input(GPIO(1, B, 7));	/* I2C0_SDA remove pull_up */
	gpio_input(GPIO(1, C, 0));	/* I2C0_SCL remove pull_up */

	i2c_init(0, 100*KHz);

	write32(&rk3399_pmugrf->iomux_i2c0_sda, IOMUX_I2C0_SDA);
	write32(&rk3399_pmugrf->iomux_i2c0_scl, IOMUX_I2C0_SCL);
}

void mainboard_power_on_backlight(void)
{
	gpio_output(GPIO_BL_EN, 1);  /* BL_EN */

	/* Configure as output GPIO, to be toggled by payload. */
	if (CONFIG(GRU_BASEBOARD_SCARLET))
		gpio_output(GPIO_BACKLIGHT, 0);

	if (CONFIG(BOARD_GOOGLE_GRU))
		prepare_backlight_i2c();
}

static struct panel_init_command innolux_p097pfg_init_cmds[] = {
	/* page 0 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00),
	MIPI_INIT_CMD(0xB1, 0xE8, 0x11),
	MIPI_INIT_CMD(0xB2, 0x25, 0x02),
	MIPI_INIT_CMD(0xB5, 0x08, 0x00),
	MIPI_INIT_CMD(0xBC, 0x0F, 0x00),
	MIPI_INIT_CMD(0xB8, 0x03, 0x06, 0x00, 0x00),
	MIPI_INIT_CMD(0xBD, 0x01, 0x90, 0x14, 0x14),
	MIPI_INIT_CMD(0x6F, 0x01),
	MIPI_INIT_CMD(0xC0, 0x03),
	MIPI_INIT_CMD(0x6F, 0x02),
	MIPI_INIT_CMD(0xC1, 0x0D),
	MIPI_INIT_CMD(0xD9, 0x01, 0x09, 0x70),
	MIPI_INIT_CMD(0xC5, 0x12, 0x21, 0x00),
	MIPI_INIT_CMD(0xBB, 0x93, 0x93),

	/* page 1 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01),
	MIPI_INIT_CMD(0xB3, 0x3C, 0x3C),
	MIPI_INIT_CMD(0xB4, 0x0F, 0x0F),
	MIPI_INIT_CMD(0xB9, 0x45, 0x45),
	MIPI_INIT_CMD(0xBA, 0x14, 0x14),
	MIPI_INIT_CMD(0xCA, 0x02),
	MIPI_INIT_CMD(0xCE, 0x04),
	MIPI_INIT_CMD(0xC3, 0x9B, 0x9B),
	MIPI_INIT_CMD(0xD8, 0xC0, 0x03),
	MIPI_INIT_CMD(0xBC, 0x82, 0x01),
	MIPI_INIT_CMD(0xBD, 0x9E, 0x01),

	/* page 2 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x02),
	MIPI_INIT_CMD(0xB0, 0x82),
	MIPI_INIT_CMD(0xD1, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x82, 0x00, 0xA5,
		      0x00, 0xC1, 0x00, 0xEA, 0x01, 0x0D, 0x01, 0x40),
	MIPI_INIT_CMD(0xD2, 0x01, 0x6A, 0x01, 0xA8, 0x01, 0xDC, 0x02, 0x29,
		      0x02, 0x67, 0x02, 0x68, 0x02, 0xA8, 0x02, 0xF0),
	MIPI_INIT_CMD(0xD3, 0x03, 0x19, 0x03, 0x49, 0x03, 0x67, 0x03, 0x8C,
		      0x03, 0xA6, 0x03, 0xC7, 0x03, 0xDE, 0x03, 0xEC),
	MIPI_INIT_CMD(0xD4, 0x03, 0xFF, 0x03, 0xFF),
	MIPI_INIT_CMD(0xE0, 0x00, 0x00, 0x00, 0x86, 0x00, 0xC5, 0x00, 0xE5,
		      0x00, 0xFF, 0x01, 0x26, 0x01, 0x45, 0x01, 0x75),
	MIPI_INIT_CMD(0xE1, 0x01, 0x9C, 0x01, 0xD5, 0x02, 0x05, 0x02, 0x4D,
		      0x02, 0x86, 0x02, 0x87, 0x02, 0xC3, 0x03, 0x03),
	MIPI_INIT_CMD(0xE2, 0x03, 0x2A, 0x03, 0x56, 0x03, 0x72, 0x03, 0x94,
		      0x03, 0xAC, 0x03, 0xCB, 0x03, 0xE0, 0x03, 0xED),
	MIPI_INIT_CMD(0xE3, 0x03, 0xFF, 0x03, 0xFF),

	/* page 3 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x03),
	MIPI_INIT_CMD(0xB0, 0x00, 0x00, 0x00, 0x00),
	MIPI_INIT_CMD(0xB1, 0x00, 0x00, 0x00, 0x00),
	MIPI_INIT_CMD(0xB2, 0x00, 0x00, 0x06, 0x04, 0x01, 0x40, 0x85),
	MIPI_INIT_CMD(0xB3, 0x10, 0x07, 0xFC, 0x04, 0x01, 0x40, 0x80),
	MIPI_INIT_CMD(0xB6, 0xF0, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01,
		      0x40, 0x80),
	MIPI_INIT_CMD(0xBA, 0xC5, 0x07, 0x00, 0x04, 0x11, 0x25, 0x8C),
	MIPI_INIT_CMD(0xBB, 0xC5, 0x07, 0x00, 0x03, 0x11, 0x25, 0x8C),
	MIPI_INIT_CMD(0xC0, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x80, 0x80),
	MIPI_INIT_CMD(0xC1, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x80, 0x80),
	MIPI_INIT_CMD(0xC4, 0x00, 0x00),
	MIPI_INIT_CMD(0xEF, 0x41),

	/* page 4 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x04),
	MIPI_INIT_CMD(0xEC, 0x4C),

	/* page 5 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x05),
	MIPI_INIT_CMD(0xB0, 0x13, 0x03, 0x03, 0x01),
	MIPI_INIT_CMD(0xB1, 0x30, 0x00),
	MIPI_INIT_CMD(0xB2, 0x02, 0x02, 0x00),
	MIPI_INIT_CMD(0xB3, 0x82, 0x23, 0x82, 0x9D),
	MIPI_INIT_CMD(0xB4, 0xC5, 0x75, 0x24, 0x57),
	MIPI_INIT_CMD(0xB5, 0x00, 0xD4, 0x72, 0x11, 0x11, 0xAB, 0x0A),
	MIPI_INIT_CMD(0xB6, 0x00, 0x00, 0xD5, 0x72, 0x24, 0x56),
	MIPI_INIT_CMD(0xB7, 0x5C, 0xDC, 0x5C, 0x5C),
	MIPI_INIT_CMD(0xB9, 0x0C, 0x00, 0x00, 0x01, 0x00),
	MIPI_INIT_CMD(0xC0, 0x75, 0x11, 0x11, 0x54, 0x05),
	MIPI_INIT_CMD(0xC6, 0x00, 0x00, 0x00, 0x00),
	MIPI_INIT_CMD(0xD0, 0x00, 0x48, 0x08, 0x00, 0x00),
	MIPI_INIT_CMD(0xD1, 0x00, 0x48, 0x09, 0x00, 0x00),

	/* page 6 */
	MIPI_INIT_CMD(0xF0, 0x55, 0xAA, 0x52, 0x08, 0x06),
	MIPI_INIT_CMD(0xB0, 0x02, 0x32, 0x32, 0x08, 0x2F),
	MIPI_INIT_CMD(0xB1, 0x2E, 0x15, 0x14, 0x13, 0x12),
	MIPI_INIT_CMD(0xB2, 0x11, 0x10, 0x00, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xB3, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xB4, 0x3D, 0x32),
	MIPI_INIT_CMD(0xB5, 0x03, 0x32, 0x32, 0x09, 0x2F),
	MIPI_INIT_CMD(0xB6, 0x2E, 0x1B, 0x1A, 0x19, 0x18),
	MIPI_INIT_CMD(0xB7, 0x17, 0x16, 0x01, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xB8, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xB9, 0x3D, 0x32),
	MIPI_INIT_CMD(0xC0, 0x01, 0x32, 0x32, 0x09, 0x2F),
	MIPI_INIT_CMD(0xC1, 0x2E, 0x1A, 0x1B, 0x16, 0x17),
	MIPI_INIT_CMD(0xC2, 0x18, 0x19, 0x03, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xC3, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xC4, 0x3D, 0x32),
	MIPI_INIT_CMD(0xC5, 0x00, 0x32, 0x32, 0x08, 0x2F),
	MIPI_INIT_CMD(0xC6, 0x2E, 0x14, 0x15, 0x10, 0x11),
	MIPI_INIT_CMD(0xC7, 0x12, 0x13, 0x02, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xC8, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D),
	MIPI_INIT_CMD(0xC9, 0x3D, 0x32),

	{},
};

static struct panel_init_command kd097d04_init_commands[] = {
	/* voltage setting */
	MIPI_INIT_CMD(0xB0, 0x00),
	MIPI_INIT_CMD(0xB2, 0x02),
	MIPI_INIT_CMD(0xB3, 0x11),
	MIPI_INIT_CMD(0xB4, 0x00),
	MIPI_INIT_CMD(0xB6, 0x80),
	/* VCOM disable */
	MIPI_INIT_CMD(0xB7, 0x02),
	MIPI_INIT_CMD(0xB8, 0x80),
	MIPI_INIT_CMD(0xBA, 0x43),
	/* VCOM setting */
	MIPI_INIT_CMD(0xBB, 0x53),
	/* VSP setting */
	MIPI_INIT_CMD(0xBC, 0x0A),
	/* VSN setting */
	MIPI_INIT_CMD(0xBD, 0x4A),
	/* VGH setting */
	MIPI_INIT_CMD(0xBE, 0x2F),
	/* VGL setting */
	MIPI_INIT_CMD(0xBF, 0x1A),
	MIPI_INIT_CMD(0xF0, 0x39),
	MIPI_INIT_CMD(0xF1, 0x22),
	/* Gamma setting */
	MIPI_INIT_CMD(0xB0, 0x02),
	MIPI_INIT_CMD(0xC0, 0x00),
	MIPI_INIT_CMD(0xC1, 0x01),
	MIPI_INIT_CMD(0xC2, 0x0B),
	MIPI_INIT_CMD(0xC3, 0x15),
	MIPI_INIT_CMD(0xC4, 0x22),
	MIPI_INIT_CMD(0xC5, 0x11),
	MIPI_INIT_CMD(0xC6, 0x15),
	MIPI_INIT_CMD(0xC7, 0x19),
	MIPI_INIT_CMD(0xC8, 0x1A),
	MIPI_INIT_CMD(0xC9, 0x16),
	MIPI_INIT_CMD(0xCA, 0x18),
	MIPI_INIT_CMD(0xCB, 0x13),
	MIPI_INIT_CMD(0xCC, 0x18),
	MIPI_INIT_CMD(0xCD, 0x13),
	MIPI_INIT_CMD(0xCE, 0x1C),
	MIPI_INIT_CMD(0xCF, 0x19),
	MIPI_INIT_CMD(0xD0, 0x21),
	MIPI_INIT_CMD(0xD1, 0x2C),
	MIPI_INIT_CMD(0xD2, 0x2F),
	MIPI_INIT_CMD(0xD3, 0x30),
	MIPI_INIT_CMD(0xD4, 0x19),
	MIPI_INIT_CMD(0xD5, 0x1F),
	MIPI_INIT_CMD(0xD6, 0x00),
	MIPI_INIT_CMD(0xD7, 0x01),
	MIPI_INIT_CMD(0xD8, 0x0B),
	MIPI_INIT_CMD(0xD9, 0x15),
	MIPI_INIT_CMD(0xDA, 0x22),
	MIPI_INIT_CMD(0xDB, 0x11),
	MIPI_INIT_CMD(0xDC, 0x15),
	MIPI_INIT_CMD(0xDD, 0x19),
	MIPI_INIT_CMD(0xDE, 0x1A),
	MIPI_INIT_CMD(0xDF, 0x16),
	MIPI_INIT_CMD(0xE0, 0x18),
	MIPI_INIT_CMD(0xE1, 0x13),
	MIPI_INIT_CMD(0xE2, 0x18),
	MIPI_INIT_CMD(0xE3, 0x13),
	MIPI_INIT_CMD(0xE4, 0x1C),
	MIPI_INIT_CMD(0xE5, 0x19),
	MIPI_INIT_CMD(0xE6, 0x21),
	MIPI_INIT_CMD(0xE7, 0x2C),
	MIPI_INIT_CMD(0xE8, 0x2F),
	MIPI_INIT_CMD(0xE9, 0x30),
	MIPI_INIT_CMD(0xEA, 0x19),
	MIPI_INIT_CMD(0xEB, 0x1F),
	/* GOA MUX setting */
	MIPI_INIT_CMD(0xB0, 0x01),
	MIPI_INIT_CMD(0xC0, 0x10),
	MIPI_INIT_CMD(0xC1, 0x0F),
	MIPI_INIT_CMD(0xC2, 0x0E),
	MIPI_INIT_CMD(0xC3, 0x0D),
	MIPI_INIT_CMD(0xC4, 0x0C),
	MIPI_INIT_CMD(0xC5, 0x0B),
	MIPI_INIT_CMD(0xC6, 0x0A),
	MIPI_INIT_CMD(0xC7, 0x09),
	MIPI_INIT_CMD(0xC8, 0x08),
	MIPI_INIT_CMD(0xC9, 0x07),
	MIPI_INIT_CMD(0xCA, 0x06),
	MIPI_INIT_CMD(0xCB, 0x05),
	MIPI_INIT_CMD(0xCC, 0x00),
	MIPI_INIT_CMD(0xCD, 0x01),
	MIPI_INIT_CMD(0xCE, 0x02),
	MIPI_INIT_CMD(0xCF, 0x03),
	MIPI_INIT_CMD(0xD0, 0x04),
	MIPI_INIT_CMD(0xD6, 0x10),
	MIPI_INIT_CMD(0xD7, 0x0F),
	MIPI_INIT_CMD(0xD8, 0x0E),
	MIPI_INIT_CMD(0xD9, 0x0D),
	MIPI_INIT_CMD(0xDA, 0x0C),
	MIPI_INIT_CMD(0xDB, 0x0B),
	MIPI_INIT_CMD(0xDC, 0x0A),
	MIPI_INIT_CMD(0xDD, 0x09),
	MIPI_INIT_CMD(0xDE, 0x08),
	MIPI_INIT_CMD(0xDF, 0x07),
	MIPI_INIT_CMD(0xE0, 0x06),
	MIPI_INIT_CMD(0xE1, 0x05),
	MIPI_INIT_CMD(0xE2, 0x00),
	MIPI_INIT_CMD(0xE3, 0x01),
	MIPI_INIT_CMD(0xE4, 0x02),
	MIPI_INIT_CMD(0xE5, 0x03),
	MIPI_INIT_CMD(0xE6, 0x04),
	MIPI_INIT_CMD(0xE7, 0x00),
	MIPI_INIT_CMD(0xEC, 0xC0),
	/* GOA timing setting */
	MIPI_INIT_CMD(0xB0, 0x03),
	MIPI_INIT_CMD(0xC0, 0x01),
	MIPI_INIT_CMD(0xC2, 0x6F),
	MIPI_INIT_CMD(0xC3, 0x6F),
	MIPI_INIT_CMD(0xC5, 0x36),
	MIPI_INIT_CMD(0xC8, 0x08),
	MIPI_INIT_CMD(0xC9, 0x04),
	MIPI_INIT_CMD(0xCA, 0x41),
	MIPI_INIT_CMD(0xCC, 0x43),
	MIPI_INIT_CMD(0xCF, 0x60),
	MIPI_INIT_CMD(0xD2, 0x04),
	MIPI_INIT_CMD(0xD3, 0x04),
	MIPI_INIT_CMD(0xD4, 0x03),
	MIPI_INIT_CMD(0xD5, 0x02),
	MIPI_INIT_CMD(0xD6, 0x01),
	MIPI_INIT_CMD(0xD7, 0x00),
	MIPI_INIT_CMD(0xDB, 0x01),
	MIPI_INIT_CMD(0xDE, 0x36),
	MIPI_INIT_CMD(0xE6, 0x6F),
	MIPI_INIT_CMD(0xE7, 0x6F),
	/* GOE setting */
	MIPI_INIT_CMD(0xB0, 0x06),
	MIPI_INIT_CMD(0xB8, 0xA5),
	MIPI_INIT_CMD(0xC0, 0xA5),
	MIPI_INIT_CMD(0xD5, 0x3F),
	{},
};

const struct mipi_panel_data kd097d04_panel = {
	.mipi_num = 2,
	.format = MIPI_DSI_FMT_RGB888,
	.lanes = 8,
	.display_on_udelay = 120000,
	.video_mode_udelay = 5000,
	.init_cmd = kd097d04_init_commands,
};

static const struct edid_mode kd097d04_edid_mode = {
	.name = "1536x2048@60Hz",
	.pixel_clock = 216000,
	.refresh = 60,
	.ha = 1536,
	.hbl = 186,
	.hso = 81,
	.hspw = 24,
	.va = 2048,
	.vbl = 42,
	.vso = 17,
	.vspw = 2,
};

const struct mipi_panel_data inx097pfg_panel = {
	.mipi_num = 2,
	.format = MIPI_DSI_FMT_RGB888,
	.lanes = 8,
	.display_on_udelay = 120000,
	.video_mode_udelay = 5000,
	.init_cmd = innolux_p097pfg_init_cmds,
};

static const struct edid_mode inx097pfg_edid_mode = {
	.name = "1536x2048@60Hz",
	.pixel_clock = 220000,
	.refresh = 60,
	.ha = 1536,
	.hbl = 224,
	.hso = 100,
	.hspw = 24,
	.va = 2048,
	.vbl = 38,
	.vso = 18,
	.vspw = 2,
};

const struct mipi_panel_data *mainboard_get_mipi_mode
				(struct edid_mode *edid_mode)
{
	switch (sku_id()) {
	case 0:
	case 2:
	case 4:
	case 6:
		memcpy(edid_mode, &inx097pfg_edid_mode,
		       sizeof(struct edid_mode));
		return &inx097pfg_panel;
	case 1:
	case 3:
	case 5:
	case 7:
	default:
		memcpy(edid_mode, &kd097d04_edid_mode,
			sizeof(struct edid_mode));
		return &kd097d04_panel;
	}
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
