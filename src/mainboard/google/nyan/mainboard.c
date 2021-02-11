/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/device.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/clk_rst.h>
#include <soc/mc.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra/usb.h>
#include <soc/pmc.h>
#include <soc/spi.h>
#include <symbols.h>
#include <vendorcode/google/chromeos/chromeos.h>

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static void set_clock_sources(void)
{
	/*
	 * The max98090 codec and the temperature sensor are on I2C1. These
	 * can both run at 400 KHz, but the kernel sets the bus to 100 KHz.
	 */
	clock_configure_i2c_scl_freq(i2c1, PLLP, 100);

	/*
	 * MMC3 and MMC4: Set base clock frequency for SD Clock to Tegra MMC's
	 * maximum speed (48MHz) so we can change SDCLK by second stage divisor
	 * in payloads, without touching base clock.
	 */
	clock_configure_source(sdmmc3, PLLP, 48000);
	clock_configure_source(sdmmc4, PLLP, 48000);

	/* External peripheral 1: audio codec (max98090) using 12MHz CLK1.
	 * Note the source id of CLK_M for EXTPERIPH1 is 3. */
	clock_configure_irregular_source(extperiph1, CLK_M, 12000, 3);

	/*
	 * We need 1.5MHz. So, we use CLK_M. CLK_DIVIDER macro returns a divisor
	 * (0xe) a little bit off from the ideal value (0xd) but it's good
	 * enough for beeps. The source id of CLK_M for I2S is 6.
	 */
	clock_configure_irregular_source(i2s1, CLK_M, 1500, 6);

	/* Note source id of PLLP for HOST1x is 4. */
	clock_configure_irregular_source(host1x, PLLP, 408000, 4);

	/* Use PLLD_OUT0 as clock source for disp1 */
	clrsetbits32(&clk_rst->clk_src_disp1,
		     CLK_SOURCE_MASK | CLK_DIVISOR_MASK,
		     2 /*PLLD_OUT0 */ << CLK_SOURCE_SHIFT);

}

static void setup_pinmux(void)
{
	// I2C1 clock.
	pinmux_set_config(PINMUX_GEN1_I2C_SCL_INDEX,
			  PINMUX_GEN1_I2C_SCL_FUNC_I2C1 | PINMUX_INPUT_ENABLE);
	// I2C1 data.
	pinmux_set_config(PINMUX_GEN1_I2C_SDA_INDEX,
			  PINMUX_GEN1_I2C_SDA_FUNC_I2C1 | PINMUX_INPUT_ENABLE);
	// I2C2 clock.
	pinmux_set_config(PINMUX_GEN2_I2C_SCL_INDEX,
			  PINMUX_GEN2_I2C_SCL_FUNC_I2C2 | PINMUX_INPUT_ENABLE |
			  PINMUX_OPEN_DRAIN);
	// I2C2 data.
	pinmux_set_config(PINMUX_GEN2_I2C_SDA_INDEX,
			  PINMUX_GEN2_I2C_SDA_FUNC_I2C2 | PINMUX_INPUT_ENABLE |
			  PINMUX_OPEN_DRAIN);
	// I2C4 (DDC) clock.
	pinmux_set_config(PINMUX_DDC_SCL_INDEX,
			  PINMUX_DDC_SCL_FUNC_I2C4 | PINMUX_INPUT_ENABLE);
	// I2C4 (DDC) data.
	pinmux_set_config(PINMUX_DDC_SDA_INDEX,
			  PINMUX_DDC_SDA_FUNC_I2C4 | PINMUX_INPUT_ENABLE);

	// TODO(hungte) Revice pinmux setup, make nice little SoC functions for
	// every single logical thing instead of dumping a wall of code below.
	uint32_t pin_up = PINMUX_PULL_UP | PINMUX_INPUT_ENABLE,
		 pin_down = PINMUX_PULL_DOWN | PINMUX_INPUT_ENABLE,
		 pin_none = PINMUX_PULL_NONE | PINMUX_INPUT_ENABLE;

	// MMC3 (sdcard reader)
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
			  PINMUX_SDMMC3_CLK_LB_IN_FUNC_SDMMC3 | pin_up);
	pinmux_set_config(PINMUX_SDMMC3_CLK_LB_OUT_INDEX,
			  PINMUX_SDMMC3_CLK_LB_OUT_FUNC_SDMMC3 | pin_down);

	// MMC3 Card Detect pin.
	gpio_input_pullup(GPIO(V2));
	// Disable SD card reader power so it can be reset even on warm boot.
	// Payloads must enable power before accessing SD card slots.
	gpio_output(GPIO(R0), 0);

	// MMC4 (eMMC)
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

	/* We pull the USB VBUS signals up but keep them as inputs since the
	 * voltage source likes to drive them low on overcurrent conditions */
	gpio_input_pullup(GPIO(N4));	/* USB VBUS EN0 */
	gpio_input_pullup(GPIO(N5));	/* USB VBUS EN1 */

	/* Clock output 1 (for external peripheral) */
	pinmux_set_config(PINMUX_DAP_MCLK1_INDEX,
			  PINMUX_DAP_MCLK1_FUNC_EXTPERIPH1 | PINMUX_PULL_NONE);

	/* I2S1 */
	pinmux_set_config(PINMUX_DAP2_DIN_INDEX,
			  PINMUX_DAP2_DIN_FUNC_I2S1 | PINMUX_INPUT_ENABLE);
	pinmux_set_config(PINMUX_DAP2_DOUT_INDEX,
			  PINMUX_DAP2_DOUT_FUNC_I2S1 | PINMUX_INPUT_ENABLE);
	pinmux_set_config(PINMUX_DAP2_FS_INDEX,
			  PINMUX_DAP2_FS_FUNC_I2S1 | PINMUX_INPUT_ENABLE);
	pinmux_set_config(PINMUX_DAP2_SCLK_INDEX,
			  PINMUX_DAP2_SCLK_FUNC_I2S1 | PINMUX_INPUT_ENABLE);

	/* PWM1 */
	pinmux_set_config(PINMUX_GPIO_PH1_INDEX,
			  PINMUX_GPIO_PH1_FUNC_PWM1 | PINMUX_PULL_NONE);

	/* DP HPD */
	pinmux_set_config(PINMUX_DP_HPD_INDEX,
			  PINMUX_DP_HPD_FUNC_DP | PINMUX_INPUT_ENABLE);
}

static void setup_kernel_info(void)
{
	// Setup required information for Linux kernel.

	// pmc.odmdata: [18:19]: console type, [15:17]: UART id.
	// TODO(hungte) This should be done by filling BCT values, or derived
	// from CONFIG_CONSOLE_SERIAL_UART[A-E]. Right now we simply copy the
	// value defined in BCT.
	struct tegra_pmc_regs *pmc = (void*)TEGRA_PMC_BASE;
	write32(&pmc->odmdata, 0x80080000);

	// Not strictly info, but kernel graphics driver needs this region locked down
	struct tegra_mc_regs *mc = (void *)TEGRA_MC_BASE;
	write32(&mc->video_protect_bom, 0);
	write32(&mc->video_protect_size_mb, 0);
	write32(&mc->video_protect_reg_ctrl, 1);
}

static void setup_ec_spi(void)
{
	tegra_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);
}

static void mainboard_init(struct device *dev)
{
	set_clock_sources();

	clock_external_output(1); /* For external MAX98090 audio codec. */

	/*
	 * Confirmed by NVIDIA hardware team, we need to take ALL audio devices
	 * conntected to AHUB (AUDIO, APBIF, I2S, DAM, AMX, ADX, SPDIF, AFC) out
	 * of reset and clock-enabled, otherwise reading AHUB devices (In our
	 * case, I2S/APBIF/AUDIO<XBAR>) will hang.
	 *
	 * Note that CLK_H_MEM (MC) and CLK_H_EMC should be already either
	 * initialized by BootROM, or in romstage SDRAM initialization.
	 */
	clock_enable_clear_reset(CLK_L_GPIO | CLK_L_I2C1 | CLK_L_SDMMC4 |
				 CLK_L_I2S0 | CLK_L_I2S1 | CLK_L_I2S2 |
				 CLK_L_SPDIF | CLK_L_USBD | CLK_L_DISP1 |
				 CLK_L_HOST1X | CLK_L_PWM,

				 CLK_H_I2C2 | CLK_H_PMC | CLK_H_USB3,

				 CLK_U_CSITE | CLK_U_SDMMC3,

				 CLK_V_I2C4 | CLK_V_EXTPERIPH1 | CLK_V_APBIF |
				 CLK_V_AUDIO | CLK_V_I2S3 | CLK_V_I2S4 |
				 CLK_V_DAM0 | CLK_V_DAM1 | CLK_V_DAM2,

				 CLK_W_DVFS | CLK_W_AMX0 | CLK_W_ADX0,

				 CLK_X_DPAUX | CLK_X_SOR0 | CLK_X_AMX1 |
				 CLK_X_ADX1 | CLK_X_AFC0 | CLK_X_AFC1 |
				 CLK_X_AFC2 | CLK_X_AFC3 | CLK_X_AFC4 |
				 CLK_X_AFC5);

	usb_setup_utmip((void*)TEGRA_USBD_BASE);
	/* USB2 is the camera, we don't need it in firmware */
	usb_setup_utmip((void*)TEGRA_USB3_BASE);

	setup_pinmux();

	i2c_init(0);
	i2c_init(1);
	i2c_init(3);

	setup_kernel_info();
	clock_init_arm_generic_timer();
	setup_ec_spi();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAG_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = REGION_SIZE(dma_coherent);
}
