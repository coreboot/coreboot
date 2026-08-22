/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootblock_common.h>
#include <soc/clk_rst.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/addressmap.h>

#include "pmic.h"

/********************** PMIC **********************************/
static const struct pad_config pmic_pads[] = {
	PAD_CFG_SFIO(PWR_I2C_SCL, PINMUX_INPUT_ENABLE, I2CPMU),
	PAD_CFG_SFIO(PWR_I2C_SDA, PINMUX_INPUT_ENABLE, I2CPMU),
};

/********************** SPI Flash *****************************/
static const struct pad_config spiflash_pads[] = {
	/* QSPI fLash: mosi, miso, clk, cs0, hold, wp  */
	PAD_CFG_SFIO(QSPI_IO0, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP |
		     PINMUX_DRIVE_2X, QSPI),
	PAD_CFG_SFIO(QSPI_IO1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP |
		     PINMUX_DRIVE_2X, QSPI),
	PAD_CFG_SFIO(QSPI_SCK, PINMUX_INPUT_ENABLE | PINMUX_DRIVE_2X, QSPI),
	PAD_CFG_SFIO(QSPI_CS_N, PINMUX_INPUT_ENABLE | PINMUX_DRIVE_2X, QSPI),
	PAD_CFG_SFIO(QSPI_IO2, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP |
		     PINMUX_DRIVE_2X, QSPI),
	PAD_CFG_SFIO(QSPI_IO3, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP |
		     PINMUX_DRIVE_2X, QSPI),
};

/********************* Funits *********************************/
static const struct funit_cfg funits[] = {
	/* PMIC on I2C5 (PWR_I2C* pads) at 400kHz. */
	FUNIT_CFG(I2C5, PLLP, 400, pmic_pads, ARRAY_SIZE(pmic_pads)),
	/* SPI flash at 24MHz on QSPI controller. */
	FUNIT_CFG(QSPI, PLLP, 24000, spiflash_pads, ARRAY_SIZE(spiflash_pads)),
};

/********************* UART ***********************************/
static const struct pad_config uart_console_pads[] = {
	/* UARTA: tx, rx, rts, cts */
	PAD_CFG_SFIO(UART1_TX, PINMUX_PULL_NONE, UARTA),
	PAD_CFG_SFIO(UART1_RX, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, UARTA),
	PAD_CFG_SFIO(UART1_RTS, PINMUX_PULL_UP, UARTA),
	PAD_CFG_SFIO(UART1_CTS, PINMUX_PULL_UP, UARTA),
};

void bootblock_mainboard_early_init(void)
{
	soc_configure_pads(uart_console_pads, ARRAY_SIZE(uart_console_pads));
}

static void set_clock_sources(void)
{
	/* UARTA gets PLLP, deactivate CLK_UART_DIV_OVERRIDE */
	write32(CLK_RST_REG(clk_src_uarta), PLLP << CLK_SOURCE_SHIFT);
}


void bootblock_mainboard_init(void)
{
	set_clock_sources();
	soc_configure_funits(funits, ARRAY_SIZE(funits));

	i2c_init(I2CPWR_BUS);
	pmic_init(I2CPWR_BUS);
}
