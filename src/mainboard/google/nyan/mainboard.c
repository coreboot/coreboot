/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/device.h>
#include <boot/coreboot_tables.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra124/clk_rst.h>
#include <soc/nvidia/tegra124/gpio.h>
#include <soc/nvidia/tegra124/pmc.h>
#include <soc/nvidia/tegra124/spi.h>
#include <soc/nvidia/tegra124/usb.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void set_clock_sources(void)
{
	clock_configure_source(i2c1, CLK_M, 1333);
	clock_configure_source(i2c2, CLK_M, 1333);
	clock_configure_source(i2c3, CLK_M, 1333);
	clock_configure_source(i2c4, CLK_M, 1333);

	clock_configure_source(sbc1, PLLP, 5000);

	/*
	 * MMC3 and MMC4: Set base clock frequency for SD Clock to Tegra MMC's
	 * maximum speed (48MHz) so we can change SDCLK by second stage divisor
	 * in payloads, without touching base clock.
	 */
	clock_configure_source(sdmmc3, PLLP, 48000);
	clock_configure_source(sdmmc4, PLLP, 48000);

	/* PLLP and PLLM are switched for HOST1x for no apparent reason. */
	write32(4 /* PLLP! */ << CLK_SOURCE_SHIFT |
		/* TODO(rminnich): The divisor isn't accurate enough to get to
		 * 144MHz (it goes to 163 instead). What should we do here? */
		CLK_DIVIDER(TEGRA_PLLP_KHZ, 144000),
		&clk_rst->clk_src_host1x);

	/* DISP1 doesn't support a divisor. Use PLLC which runs at 600MHz. */
	clock_configure_source(disp1, PLLC, 600000);
}

static void setup_pinmux(void)
{
	// Write protect.
	gpio_input_pullup(GPIO(R1));
	// Recovery mode.
	gpio_input_pullup(GPIO(Q7));
	// Lid switch.
	gpio_input_pullup(GPIO(R4));
	// Power switch.
	gpio_input_pullup(GPIO(Q0));
	// Developer mode.
	gpio_input_pullup(GPIO(Q6));
	// EC in RW.
	gpio_input_pullup(GPIO(U4));

	// SPI1 MOSI
	pinmux_set_config(PINMUX_ULPI_CLK_INDEX, PINMUX_ULPI_CLK_FUNC_SPI1 |
						 PINMUX_PULL_UP |
						 PINMUX_INPUT_ENABLE);
	// SPI1 MISO
	pinmux_set_config(PINMUX_ULPI_DIR_INDEX, PINMUX_ULPI_DIR_FUNC_SPI1 |
						 PINMUX_PULL_UP |
						 PINMUX_INPUT_ENABLE);
	// SPI1 SCLK
	pinmux_set_config(PINMUX_ULPI_NXT_INDEX, PINMUX_ULPI_NXT_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);
	// SPI1 CS0
	pinmux_set_config(PINMUX_ULPI_STP_INDEX, PINMUX_ULPI_STP_FUNC_SPI1 |
						 PINMUX_PULL_NONE |
						 PINMUX_INPUT_ENABLE);

	// I2C1 clock.
	pinmux_set_config(PINMUX_GEN1_I2C_SCL_INDEX,
			  PINMUX_GEN1_I2C_SCL_FUNC_I2C1 | PINMUX_INPUT_ENABLE);
	// I2C1 data.
	pinmux_set_config(PINMUX_GEN1_I2C_SDA_INDEX,
			  PINMUX_GEN1_I2C_SDA_FUNC_I2C1 | PINMUX_INPUT_ENABLE);
	// I2C2 clock.
	pinmux_set_config(PINMUX_GEN2_I2C_SCL_INDEX,
			  PINMUX_GEN2_I2C_SCL_FUNC_I2C2 | PINMUX_INPUT_ENABLE);
	// I2C2 data.
	pinmux_set_config(PINMUX_GEN2_I2C_SDA_INDEX,
			  PINMUX_GEN2_I2C_SDA_FUNC_I2C2 | PINMUX_INPUT_ENABLE);
	// I2C3 (cam) clock.
	pinmux_set_config(PINMUX_CAM_I2C_SCL_INDEX,
			  PINMUX_CAM_I2C_SCL_FUNC_I2C3 | PINMUX_INPUT_ENABLE);
	// I2C3 (cam) data.
	pinmux_set_config(PINMUX_CAM_I2C_SDA_INDEX,
			  PINMUX_CAM_I2C_SDA_FUNC_I2C3 | PINMUX_INPUT_ENABLE);
	// I2C4 (DDC) clock.
	pinmux_set_config(PINMUX_DDC_SCL_INDEX,
			  PINMUX_DDC_SCL_FUNC_I2C4 | PINMUX_INPUT_ENABLE);
	// I2C4 (DDC) data.
	pinmux_set_config(PINMUX_DDC_SDA_INDEX,
			  PINMUX_DDC_SDA_FUNC_I2C4 | PINMUX_INPUT_ENABLE);

	// TODO(hungte) Revice pinmux setup, make nice little SoC functions for
	// every single logical thing instead of dumping a wall of code below.
	uint32_t pin_up = PINMUX_PULL_UP | PINMUX_INPUT_ENABLE,
		 pin_up3 = (PINMUX_PULL_UP | PINMUX_INPUT_ENABLE |
			    PINMUX_TRISTATE),
		 pin_down = PINMUX_PULL_DOWN | PINMUX_INPUT_ENABLE,
		 pin_none = PINMUX_PULL_NONE | PINMUX_INPUT_ENABLE;

	// MMC3
	pinmux_set_config(PINMUX_SDMMC3_CLK_INDEX,
			  PINMUX_SDMMC3_CLK_FUNC_SDMMC3 | pin_none);
	pinmux_set_config(PINMUX_SDMMC3_CMD_INDEX,
			  PINMUX_SDMMC3_CMD_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT0_INDEX,
			  PINMUX_SDMMC3_DAT0_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT1_INDEX,
			  PINMUX_SDMMC3_DAT1_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT2_INDEX,
			  PINMUX_SDMMC3_DAT2_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_DAT3_INDEX,
			  PINMUX_SDMMC3_DAT3_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_CLK_LB_IN_INDEX,
			  PINMUX_SDMMC3_CLK_LB_IN_FUNC_SDMMC3 | pin_up3);
	pinmux_set_config(PINMUX_SDMMC3_CLK_LB_OUT_INDEX,
			  PINMUX_SDMMC3_CLK_LB_OUT_FUNC_SDMMC3 | pin_down);

	// MMC3 Card Detect pin.
	gpio_input_pullup(GPIO(V2));
	// Enable MMC power.
	gpio_output(GPIO(R0), 1);

	// MMC4
	pinmux_set_config(PINMUX_SDMMC4_CLK_INDEX,
			  PINMUX_SDMMC4_CLK_FUNC_SDMMC4 | pin_none);
	pinmux_set_config(PINMUX_SDMMC4_CMD_INDEX,
			  PINMUX_SDMMC4_CMD_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT0_INDEX,
			  PINMUX_SDMMC4_DAT0_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT1_INDEX,
			  PINMUX_SDMMC4_DAT1_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT2_INDEX,
			  PINMUX_SDMMC4_DAT2_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT3_INDEX,
			  PINMUX_SDMMC4_DAT3_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT4_INDEX,
			  PINMUX_SDMMC4_DAT4_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT5_INDEX,
			  PINMUX_SDMMC4_DAT5_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT6_INDEX,
			  PINMUX_SDMMC4_DAT6_FUNC_SDMMC4 | pin_up);
	pinmux_set_config(PINMUX_SDMMC4_DAT7_INDEX,
			  PINMUX_SDMMC4_DAT7_FUNC_SDMMC4 | pin_up);

	/* TODO: This is supposed to work with the USB special function pinmux,
	 * but it doesn't. Go with GPIOs for now and solve the problem later. */
	gpio_output_open_drain(GPIO(N4), 1);	/* USB VBUS EN0 */
	gpio_output_open_drain(GPIO(N5), 1);	/* USB VBUS EN1 */
}

static void setup_kernel_info(void)
{
	// Setup required information for Linux kernel.

	// pmc.odmdata: [18:19]: console type, [15:17]: UART id.
	// TODO(hungte) This should be done by filling BCT values, or derived
	// from CONFIG_CONSOLE_SERIAL_UART[A-E]. Right now we simply copy the
	// value defined in BCT.
	struct tegra_pmc_regs *pmc = (void*)TEGRA_PMC_BASE;
	writel(0x80080000, &pmc->odmdata);
}

static void setup_ec_spi(void)
{
	struct tegra_spi_channel *spi;

	spi = tegra_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);

	/* Set frame header for use by CrOS EC */
	spi->frame_header = 0xec;
	spi->rx_frame_header_enable = 1;
}

static void mainboard_init(device_t dev)
{
	set_clock_sources();
	clock_enable_clear_reset(CLK_L_GPIO | CLK_L_I2C1 |
				 CLK_L_SDMMC4 | CLK_L_USBD,
				 CLK_H_EMC | CLK_H_I2C2 | CLK_H_SBC1 |
				 CLK_H_PMC | CLK_H_MEM | CLK_H_USB3,
				 CLK_U_I2C3 | CLK_U_CSITE | CLK_U_SDMMC3,
				 CLK_V_I2C4,
				 CLK_W_DVFS);

	usb_setup_utmip1();
	/* USB2 is the camera, we don't need it in firmware */
	usb_setup_utmip3();

	setup_pinmux();

	i2c_init(0);
	i2c_init(1);
	i2c_init(2);
	i2c_init(3);

	setup_kernel_info();
	clock_init_arm_generic_timer();
	setup_ec_spi();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name	= "nyan",
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAB_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = CONFIG_DRAM_DMA_START;
	dma->range_size = CONFIG_DRAM_DMA_SIZE;
}
