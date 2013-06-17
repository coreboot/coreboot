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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "pinmux.h"

#include <arch/io.h>
#include <config.h>

static struct am335x_pinmux_regs *regs =
	(struct am335x_pinmux_regs *)(uintptr_t)AM335X_PINMUX_REG_ADDR;

void am335x_pinmux_uart0(void)
{
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->uart0_rxd);
	writel(MODE(0) | PULLUDEN, &regs->uart0_txd);
}

void am335x_pinmux_uart1(void)
{
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->uart1_rxd);
	writel(MODE(0) | PULLUDEN, &regs->uart1_txd);
}

void am335x_pinmux_uart2(void)
{
	// UART2_RXD
	writel(MODE(1) | PULLUP_EN | RXACTIVE, &regs->spi0_sclk);
	// UART2_TXD
	writel(MODE(1) | PULLUDEN, &regs->spi0_d0);
}

void am335x_pinmux_uart3(void)
{
	// UART3_RXD
	writel(MODE(1) | PULLUP_EN | RXACTIVE, &regs->spi0_cs1);
	// UART3_TXD
	writel(MODE(1) | PULLUDEN, &regs->ecap0_in_pwm0_out);
}

void am335x_pinmux_uart4(void)
{
	// UART4_RXD
	writel(MODE(6) | PULLUP_EN | RXACTIVE, &regs->gpmc_wait0);
	// UART4_TXD
	writel(MODE(6) | PULLUDEN, &regs->gpmc_wpn);
}

void am335x_pinmux_uart5(void)
{
	// UART5_RXD
	writel(MODE(4) | PULLUP_EN | RXACTIVE, &regs->lcd_data9);
	// UART5_TXD
	writel(MODE(4) | PULLUDEN, &regs->lcd_data8);
}

void am335x_pinmux_mmc0(int cd, int sk_evm)
{
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mmc0_dat0);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mmc0_dat1);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mmc0_dat2);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mmc0_dat3);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mmc0_clk);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mmc0_cmd);
	if (!sk_evm) {
		// MMC0_WP
		writel(MODE(4) | RXACTIVE, &regs->mcasp0_aclkr);
	}
	if (cd) {
		// MMC0_CD
		writel(MODE(5) | RXACTIVE | PULLUP_EN, &regs->spi0_cs1);
	}
}

void am335x_pinmux_mmc1(void)
{
	// MMC1_DAT0
	writel(MODE(1) | RXACTIVE | PULLUP_EN, &regs->gpmc_ad0);
	// MMC1_DAT1
	writel(MODE(1) | RXACTIVE | PULLUP_EN, &regs->gpmc_ad1);
	// MMC1_DAT2
	writel(MODE(1) | RXACTIVE | PULLUP_EN, &regs->gpmc_ad2);
	// MMC1_DAT3
	writel(MODE(1) | RXACTIVE | PULLUP_EN, &regs->gpmc_ad3);
	// MMC1_CLK
	writel(MODE(2) | RXACTIVE | PULLUP_EN, &regs->gpmc_csn1);
	// MMC1_CMD
	writel(MODE(2) | RXACTIVE | PULLUP_EN, &regs->gpmc_csn2);
	// MMC1_WP
	writel(MODE(7) | RXACTIVE | PULLUP_EN, &regs->gpmc_csn0);
	// MMC1_CD
	writel(MODE(7) | RXACTIVE | PULLUP_EN, &regs->gpmc_advn_ale);
}

void am335x_pinmux_i2c0(void)
{
	writel(MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL, &regs->i2c0_sda);
	writel(MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL, &regs->i2c0_scl);
}

void am335x_pinmux_i2c1(void)
{
	// I2C_DATA
	writel(MODE(2) | RXACTIVE | PULLUDEN | SLEWCTRL, &regs->spi0_d1);
	// I2C_SCLK
	writel(MODE(2) | RXACTIVE | PULLUDEN | SLEWCTRL, &regs->spi0_cs0);
}

void am335x_pinmux_spi0(void)
{
	writel(MODE(0) | RXACTIVE | PULLUDEN, &regs->spi0_sclk);
	writel(MODE(0) | RXACTIVE | PULLUDEN | PULLUP_EN, &regs->spi0_d0);
	writel(MODE(0) | RXACTIVE | PULLUDEN, &regs->spi0_d1);
	writel(MODE(0) | RXACTIVE | PULLUDEN | PULLUP_EN, &regs->spi0_cs0);
}

void am335x_pinmux_gpio0_7(void)
{
	writel(MODE(7) | PULLUDEN, &regs->ecap0_in_pwm0_out);
}

void am335x_pinmux_rgmii1(void)
{
	writel(MODE(2), &regs->mii1_txen);
	writel(MODE(2) | RXACTIVE, &regs->mii1_rxdv);
	writel(MODE(2), &regs->mii1_txd0);
	writel(MODE(2), &regs->mii1_txd1);
	writel(MODE(2), &regs->mii1_txd2);
	writel(MODE(2), &regs->mii1_txd3);
	writel(MODE(2), &regs->mii1_txclk);
	writel(MODE(2) | RXACTIVE, &regs->mii1_rxclk);
	writel(MODE(2) | RXACTIVE, &regs->mii1_rxd0);
	writel(MODE(2) | RXACTIVE, &regs->mii1_rxd1);
	writel(MODE(2) | RXACTIVE, &regs->mii1_rxd2);
	writel(MODE(2) | RXACTIVE, &regs->mii1_rxd3);
}

void am335x_pinmux_mii1(void)
{
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxerr);
	writel(MODE(0), &regs->mii1_txen);
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxdv);
	writel(MODE(0), &regs->mii1_txd0);
	writel(MODE(0), &regs->mii1_txd1);
	writel(MODE(0), &regs->mii1_txd2);
	writel(MODE(0), &regs->mii1_txd3);
	writel(MODE(0) | RXACTIVE, &regs->mii1_txclk);
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxclk);
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxd0);
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxd1);
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxd2);
	writel(MODE(0) | RXACTIVE, &regs->mii1_rxd3);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->mdio_data);
	writel(MODE(0) | PULLUP_EN, &regs->mdio_clk);
}

void am335x_pinmux_nand(void)
{
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad0);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad1);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad2);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad3);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad4);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad5);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad6);
	writel(MODE(0) | PULLUP_EN | RXACTIVE, &regs->gpmc_ad7);
	writel(MODE(0) | RXACTIVE | PULLUP_EN, &regs->gpmc_wait0);
	writel(MODE(7) | PULLUP_EN | RXACTIVE, &regs->gpmc_wpn);
	writel(MODE(0) | PULLUDEN, &regs->gpmc_csn0);
	writel(MODE(0) | PULLUDEN, &regs->gpmc_advn_ale);
	writel(MODE(0) | PULLUDEN, &regs->gpmc_oen_ren);
	writel(MODE(0) | PULLUDEN, &regs->gpmc_wen);
	writel(MODE(0) | PULLUDEN, &regs->gpmc_be0n_cle);
}
