/*
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "pinmux.h"

#include <arch/io.h>

static struct am335x_pinmux_regs *regs =
	(struct am335x_pinmux_regs *)(uintptr_t)AM335X_PINMUX_REG_ADDR;

void am335x_pinmux_uart0(void)
{
	write32(&regs->uart0_rxd, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->uart0_txd, MODE(0) | PULLUDEN);
}

void am335x_pinmux_uart1(void)
{
	write32(&regs->uart1_rxd, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->uart1_txd, MODE(0) | PULLUDEN);
}

void am335x_pinmux_uart2(void)
{
	// UART2_RXD
	write32(&regs->spi0_sclk, MODE(1) | PULLUP_EN | RXACTIVE);
	// UART2_TXD
	write32(&regs->spi0_d0, MODE(1) | PULLUDEN);
}

void am335x_pinmux_uart3(void)
{
	// UART3_RXD
	write32(&regs->spi0_cs1, MODE(1) | PULLUP_EN | RXACTIVE);
	// UART3_TXD
	write32(&regs->ecap0_in_pwm0_out, MODE(1) | PULLUDEN);
}

void am335x_pinmux_uart4(void)
{
	// UART4_RXD
	write32(&regs->gpmc_wait0, MODE(6) | PULLUP_EN | RXACTIVE);
	// UART4_TXD
	write32(&regs->gpmc_wpn, MODE(6) | PULLUDEN);
}

void am335x_pinmux_uart5(void)
{
	// UART5_RXD
	write32(&regs->lcd_data9, MODE(4) | PULLUP_EN | RXACTIVE);
	// UART5_TXD
	write32(&regs->lcd_data8, MODE(4) | PULLUDEN);
}

void am335x_pinmux_mmc0(int cd, int sk_evm)
{
	write32(&regs->mmc0_dat0, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->mmc0_dat1, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->mmc0_dat2, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->mmc0_dat3, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->mmc0_clk, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->mmc0_cmd, MODE(0) | RXACTIVE | PULLUP_EN);
	if (!sk_evm) {
		// MMC0_WP
		write32(&regs->mcasp0_aclkr, MODE(4) | RXACTIVE);
	}
	if (cd) {
		// MMC0_CD
		write32(&regs->spi0_cs1, MODE(5) | RXACTIVE | PULLUP_EN);
	}
}

void am335x_pinmux_mmc1(void)
{
	// MMC1_DAT0
	write32(&regs->gpmc_ad0, MODE(1) | RXACTIVE | PULLUP_EN);
	// MMC1_DAT1
	write32(&regs->gpmc_ad1, MODE(1) | RXACTIVE | PULLUP_EN);
	// MMC1_DAT2
	write32(&regs->gpmc_ad2, MODE(1) | RXACTIVE | PULLUP_EN);
	// MMC1_DAT3
	write32(&regs->gpmc_ad3, MODE(1) | RXACTIVE | PULLUP_EN);
	// MMC1_CLK
	write32(&regs->gpmc_csn1, MODE(2) | RXACTIVE | PULLUP_EN);
	// MMC1_CMD
	write32(&regs->gpmc_csn2, MODE(2) | RXACTIVE | PULLUP_EN);
	// MMC1_WP
	write32(&regs->gpmc_csn0, MODE(7) | RXACTIVE | PULLUP_EN);
	// MMC1_CD
	write32(&regs->gpmc_advn_ale, MODE(7) | RXACTIVE | PULLUP_EN);
}

void am335x_pinmux_i2c0(void)
{
	write32(&regs->i2c0_sda, MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL);
	write32(&regs->i2c0_scl, MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL);
}

void am335x_pinmux_i2c1(void)
{
	// I2C_DATA
	write32(&regs->spi0_d1, MODE(2) | RXACTIVE | PULLUDEN | SLEWCTRL);
	// I2C_SCLK
	write32(&regs->spi0_cs0, MODE(2) | RXACTIVE | PULLUDEN | SLEWCTRL);
}

void am335x_pinmux_spi0(void)
{
	write32(&regs->spi0_sclk, MODE(0) | RXACTIVE | PULLUDEN);
	write32(&regs->spi0_d0, MODE(0) | RXACTIVE | PULLUDEN | PULLUP_EN);
	write32(&regs->spi0_d1, MODE(0) | RXACTIVE | PULLUDEN);
	write32(&regs->spi0_cs0, MODE(0) | RXACTIVE | PULLUDEN | PULLUP_EN);
}

void am335x_pinmux_gpio0_7(void)
{
	write32(&regs->ecap0_in_pwm0_out, MODE(7) | PULLUDEN);
}

void am335x_pinmux_rgmii1(void)
{
	write32(&regs->mii1_txen, MODE(2));
	write32(&regs->mii1_rxdv, MODE(2) | RXACTIVE);
	write32(&regs->mii1_txd0, MODE(2));
	write32(&regs->mii1_txd1, MODE(2));
	write32(&regs->mii1_txd2, MODE(2));
	write32(&regs->mii1_txd3, MODE(2));
	write32(&regs->mii1_txclk, MODE(2));
	write32(&regs->mii1_rxclk, MODE(2) | RXACTIVE);
	write32(&regs->mii1_rxd0, MODE(2) | RXACTIVE);
	write32(&regs->mii1_rxd1, MODE(2) | RXACTIVE);
	write32(&regs->mii1_rxd2, MODE(2) | RXACTIVE);
	write32(&regs->mii1_rxd3, MODE(2) | RXACTIVE);
}

void am335x_pinmux_mii1(void)
{
	write32(&regs->mii1_rxerr, MODE(0) | RXACTIVE);
	write32(&regs->mii1_txen, MODE(0));
	write32(&regs->mii1_rxdv, MODE(0) | RXACTIVE);
	write32(&regs->mii1_txd0, MODE(0));
	write32(&regs->mii1_txd1, MODE(0));
	write32(&regs->mii1_txd2, MODE(0));
	write32(&regs->mii1_txd3, MODE(0));
	write32(&regs->mii1_txclk, MODE(0) | RXACTIVE);
	write32(&regs->mii1_rxclk, MODE(0) | RXACTIVE);
	write32(&regs->mii1_rxd0, MODE(0) | RXACTIVE);
	write32(&regs->mii1_rxd1, MODE(0) | RXACTIVE);
	write32(&regs->mii1_rxd2, MODE(0) | RXACTIVE);
	write32(&regs->mii1_rxd3, MODE(0) | RXACTIVE);
	write32(&regs->mdio_data, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->mdio_clk, MODE(0) | PULLUP_EN);
}

void am335x_pinmux_nand(void)
{
	write32(&regs->gpmc_ad0, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad1, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad2, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad3, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad4, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad5, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad6, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_ad7, MODE(0) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_wait0, MODE(0) | RXACTIVE | PULLUP_EN);
	write32(&regs->gpmc_wpn, MODE(7) | PULLUP_EN | RXACTIVE);
	write32(&regs->gpmc_csn0, MODE(0) | PULLUDEN);
	write32(&regs->gpmc_advn_ale, MODE(0) | PULLUDEN);
	write32(&regs->gpmc_oen_ren, MODE(0) | PULLUDEN);
	write32(&regs->gpmc_wen, MODE(0) | PULLUDEN);
	write32(&regs->gpmc_be0n_cle, MODE(0) | PULLUDEN);
}
