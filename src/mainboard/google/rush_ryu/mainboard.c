/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/mmu.h>
#include <boardid.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <cbmem.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
#include <elog.h>
#include <memrange.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/display.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <vendorcode/google/chromeos/cros_vpd.h>
#if IS_ENABLED(CONFIG_CHROMEOS)
#include <vboot_struct.h>
#include <vendorcode/google/chromeos/vboot_handoff.h>
#include <vendorcode/google/chromeos/vboot2/misc.h>
#endif

#include "gpio.h"
#include "pmic.h"

static const struct pad_config mmcpads[] = {
	/* MMC4 (eMMC) */
	PAD_CFG_SFIO(SDMMC4_CLK, PINMUX_INPUT_ENABLE|PINMUX_PULL_DOWN, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_CMD, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT0, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT1, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT2, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT3, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT4, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT5, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT6, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
	PAD_CFG_SFIO(SDMMC4_DAT7, PINMUX_INPUT_ENABLE|PINMUX_PULL_UP, SDMMC4),
};

static const struct pad_config audio_codec_pads[] = {
	/* H1 is CODEC_RST_L and R2(ROW2) is AUDIO_ENABLE */
	PAD_CFG_GPIO_OUT1(GPIO_PH1, PINMUX_PULL_DOWN),
	PAD_CFG_GPIO_OUT1(KB_ROW2, PINMUX_PULL_DOWN),
};

static const struct funit_cfg funits[] = {
	/* MMC on SDMMC4 controller at 48MHz. */
	FUNIT_CFG(SDMMC4, PLLP, 48000, mmcpads, ARRAY_SIZE(mmcpads)),
	/* I2C6 for audio, temp sensor, etc. Enable codec via GPIOs/muxes */
	FUNIT_CFG(I2C6, PLLP, 400, audio_codec_pads, ARRAY_SIZE(audio_codec_pads)),
	FUNIT_CFG_USB(USBD),
};

/* HACK: For proto boards before proto3, we want to disable ec sw sync */
static void fix_ec_sw_sync(void)
{
#if IS_ENABLED(CONFIG_CHROMEOS)
	struct vboot_handoff *vh;

	if (board_id() >= BOARD_ID_PROTO_3)
		return;

	vh = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vh == NULL) {
		printk(BIOS_ERR, "No vboot handoff struct found\n");
		return;
	}

	VbSharedDataHeader *vb_sd = (VbSharedDataHeader *)vh->shared_data;
	vb_sd->flags &= ~VBSD_EC_SOFTWARE_SYNC;
#endif
}

static const struct pad_config lcd_gpio_padcfgs[] = {
	/* LCD_EN */
	PAD_CFG_GPIO_OUT0(GPIO_PH5, PINMUX_PULL_UP),
	/* LCD_RST_L */
	PAD_CFG_GPIO_OUT0(GPIO_PH3, PINMUX_PULL_UP),
	/* EN_VDD_LCD */
	PAD_CFG_GPIO_OUT0(GPIO_PBB6, PINMUX_PULL_NONE),
	/* EN_VDD18_LCD */
	PAD_CFG_GPIO_OUT0(DVFS_PWM, PINMUX_PULL_DOWN),
};

static void configure_display_clocks(void)
{
	u32 lclks = CLK_L_HOST1X | CLK_L_DISP1;	/* dc */
	u32 hclks = CLK_H_MIPI_CAL | CLK_H_DSI; /* mipi phy, mipi-dsi a */
	u32 uclks = CLK_U_DSIB;			/* mipi-dsi b */
	u32 xclks = CLK_X_CLK72MHZ;		/* clk src of mipi_cal */

	clock_enable_clear_reset(lclks, hclks, uclks, 0, 0, xclks);

	/* Give clocks time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);
}

static int enable_lcd_vdd(void)
{
	uint8_t data;

	/* Set 1.20V to power AVDD_DSI_CSI */
	pmic_write_reg(I2CPWR_BUS, TI65913_LDO5_VOLTAGE,
			VSEL_1200, 1);
	pmic_write_reg(I2CPWR_BUS, TI65913_LDO5_CTRL,
			TI65913_MODE_ACTIVE_ON, 1);

	/*
	 * Enable VDD_LCD
	 *
	 * Use different GPIO based on board id
	 */
	switch (board_id()) {
	case BOARD_ID_PROTO_0:
		/* Select PMIC GPIO_6's primary function */
		pmic_read_reg(I2CPWR_BUS, TI65913_PAD2, &data);
		pmic_write_reg(I2CPWR_BUS, TI65913_PAD2,
				 PAD2_GPIO_6_PRIMARY(data), 0);

		/* Set PMIC_GPIO_6 as output */
		pmic_read_reg(I2CPWR_BUS, TI65913_GPIO_DATA_DIR, &data);
		pmic_write_reg(I2CPWR_BUS, TI65913_GPIO_DATA_DIR,
				TI65913_GPIO_6_OUTPUT, 0);

		/* Set PMIC_GPIO_6 output high */
		pmic_read_reg(I2CPWR_BUS, TI65913_GPIO_DATA_OUT, &data);
		pmic_write_reg(I2CPWR_BUS, TI65913_GPIO_DATA_OUT,
				TI65913_GPIO_6_HIGH, 1);
		break;
	case BOARD_ID_PROTO_1:
	case BOARD_ID_PROTO_3:
	case BOARD_ID_PROTO_4:
	case BOARD_ID_EVT:
		gpio_set(EN_VDD_LCD, 1);
		break;
	default: /* unknown board */
		return -1;
	}
	/* wait for 2ms */
	mdelay(2);

	/* Enable PP1800_LCDIO to panel */
	gpio_set(EN_VDD18_LCD, 1);
	/* wait for 1ms */
	mdelay(1);

	/* Set panel EN and RST signals */
	gpio_set(LCD_EN, 1);		/* enable */
	/* wait for min 10ms */
	mdelay(10);
	gpio_set(LCD_RST_L, 1);		/* clear reset */
	/* wait for min 3ms */
	mdelay(3);

	return 0;
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

static int configure_display_blocks(void)
{
	/* set and enable panel related vdd */
	if (enable_lcd_vdd())
		return -1;

	/* enable display related clocks */
	configure_display_clocks();

	return 0;
}

/* Audio init: clocks and enables/resets */
static void setup_audio(void)
{
	/*
	 * External peripheral 1: audio codec (RT5677) uses 12MHz CLK1
	 * NOTE: We can't use a funits struct/call here because EXTPERIPH1/2/3
	 * don't have BASE regs or CAR RST/ENA bits. Also, the mux setting for
	 * EXTPERIPH1/DAP_MCLK1 is rolled into the I2S1 padcfg.
	 */
	clock_configure_source(extperiph1, CLK_M, 12000);

	soc_configure_funits(audio_funit, ARRAY_SIZE(audio_funit));

	clock_external_output(1);	/* For external RT5677 audio codec. */

	/*
	 * Confirmed by NVIDIA hardware team, we need to take ALL audio devices
	 * connected to AHUB (AUDIO, APBIF, I2S, DAM, AMX, ADX, SPDIF, AFC) out
	 * of reset and clock-enabled, otherwise reading AHUB devices (in our
	 * case, I2S/APBIF/AUDIO<XBAR>) will hang.
	 */
	clock_enable_audio();
}

#define AD4567_DEV	0x34
#define PWR_CTL		0
#define DAC_CTL		2
#define SPWDN		(1 << 0)
#define DAC_MUTE	(1 << 6)
#define DAC_FS		(0x7 << 0)
#define SR_32K_48KHZ	0x2

static void enable_ad4567_spkr_amp(void)
{
	uint8_t reg_byte;

	if (board_id() >= BOARD_ID_PROTO_3)
		return;
	/*
	 * I2C6, device 0x34 is an AD4567 speaker amp on P0/P1.
	 * It needs to have a couple of regs tweaked to turn it on
	 * so it can provide audio output to the mono speaker on P0/P1.
	 */
	i2c_readb(I2C6_BUS, AD4567_DEV, PWR_CTL, &reg_byte);
	reg_byte &= ~SPWDN;		// power up amp
	i2c_writeb(I2C6_BUS, AD4567_DEV, PWR_CTL, reg_byte);

	/* The next 2 settings are defaults, but set them anyway */
	i2c_readb(I2C6_BUS, AD4567_DEV, DAC_CTL, &reg_byte);
	reg_byte &= ~DAC_MUTE;		// unmute DAC (default)
	reg_byte &= ~DAC_FS;		// mask sample rate bits
	reg_byte |= SR_32K_48KHZ;	// set 32K-48KHz sample rate (default)
	i2c_writeb(I2C6_BUS, AD4567_DEV, DAC_CTL, reg_byte);
}

static void mainboard_init(device_t dev)
{
	soc_configure_funits(funits, ARRAY_SIZE(funits));

	/* I2C6 bus (audio, etc.) */
	soc_configure_i2c6pad();
	i2c_init(I2C6_BUS);

	setup_audio();
	/* Temp hack for P1 board: Enable speaker amp (powerup, etc.) */
	enable_ad4567_spkr_amp();

	elog_init();
	elog_add_boot_reason();

	fix_ec_sw_sync();

	/* configure panel gpio pads */
	soc_configure_pads(lcd_gpio_padcfgs, ARRAY_SIZE(lcd_gpio_padcfgs));

	/* if panel needs to bringup */
	if (display_init_required())
		configure_display_blocks();
}

void display_startup(device_t dev)
{
	dsi_display_startup(dev);
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name   = "rush_ryu",
	.enable_dev = mainboard_enable,
};

#if IS_ENABLED(CONFIG_CHROMEOS)
void lb_board(struct lb_header *header)
{
	lb_table_add_serialno_from_vpd(header);
}
#endif
