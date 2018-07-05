/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
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

#include <delay.h>
#include <soc/clock.h>
#include <types.h>

/**
 * uart_pll_vote_clk_enable - enables PLL8
 */
void uart_pll_vote_clk_enable(unsigned int clk_dummy)
{
	setbits_le32(BB_PLL_ENA_SC0_REG, BIT(8));

	if (!clk_dummy)
		while ((read32(PLL_LOCK_DET_STATUS_REG) & BIT(8)) == 0);
}

/**
 * uart_set_rate_mnd - configures divider M and D values
 *
 * Sets the M, D parameters of the divider to generate the GSBI UART
 * apps clock.
 */
static void uart_set_rate_mnd(unsigned int gsbi_port, unsigned int m,
		unsigned int n)
{
	/* Assert MND reset. */
	setbits_le32(GSBIn_UART_APPS_NS_REG(gsbi_port), BIT(7));
	/* Program M and D values. */
	write32(GSBIn_UART_APPS_MD_REG(gsbi_port), MD16(m, n));
	/* Deassert MND reset. */
	clrbits_le32(GSBIn_UART_APPS_NS_REG(gsbi_port), BIT(7));
}

/**
 * uart_branch_clk_enable_reg - enables branch clock
 *
 * Enables branch clock for GSBI UART port.
 */
static void uart_branch_clk_enable_reg(unsigned int gsbi_port)
{
	setbits_le32(GSBIn_UART_APPS_NS_REG(gsbi_port), BIT(9));
}

/**
 * uart_local_clock_enable - configures N value and enables root clocks
 *
 * Sets the N parameter of the divider and enables root clock and
 * branch clocks for GSBI UART port.
 */
static void uart_local_clock_enable(unsigned int gsbi_port, unsigned int n,
					unsigned int m)
{
	unsigned int reg_val, uart_ns_val;
	void *const reg = (void *)GSBIn_UART_APPS_NS_REG(gsbi_port);

	/*
	* Program the NS register, if applicable. NS registers are not
	* set in the set_rate path because power can be saved by deferring
	* the selection of a clocked source until the clock is enabled.
	*/
	reg_val = read32(reg); // REG(0x29D4+(0x20*((n)-1)))
	reg_val &= ~(Uart_clk_ns_mask);
	uart_ns_val =  NS(BIT_POS_31,BIT_POS_16,n,m, 5, 4, 3, 1, 2, 0,3);
	reg_val |= (uart_ns_val & Uart_clk_ns_mask);
	write32(reg, reg_val);

	/* enable MNCNTR_EN */
	reg_val = read32(reg);
	reg_val |= BIT(8);
	write32(reg, reg_val);

	/* set source to PLL8 running @384MHz */
	reg_val = read32(reg);
	reg_val |= 0x3;
	write32(reg, reg_val);

	/* Enable root. */
	reg_val |= Uart_en_mask;
	write32(reg, reg_val);
	uart_branch_clk_enable_reg(gsbi_port);
}

/**
 * uart_set_gsbi_clk - enables HCLK for UART GSBI port
 */
static void uart_set_gsbi_clk(unsigned int gsbi_port)
{
	setbits_le32(GSBIn_HCLK_CTL_REG(gsbi_port), BIT(4));
}

/**
 * uart_clock_config - configures UART clocks
 *
 * Configures GSBI UART dividers, enable root and branch clocks.
 */
void uart_clock_config(unsigned int gsbi_port, unsigned int m,
		unsigned int n, unsigned int d, unsigned int clk_dummy)
{
	uart_set_rate_mnd(gsbi_port, m, d);
	uart_pll_vote_clk_enable(clk_dummy);
	uart_local_clock_enable(gsbi_port, n, m);
	uart_set_gsbi_clk(gsbi_port);
}

/**
 * nand_clock_config - configure NAND controller clocks
 *
 * Enable clocks to EBI2. Must be invoked before touching EBI2
 * registers.
 */
void nand_clock_config(void)
{
	write32(EBI2_CLK_CTL_REG,
		CLK_BRANCH_ENA(1) | ALWAYS_ON_CLK_BRANCH_ENA(1));

	/* Wait for clock to stabilize. */
	udelay(10);
}

/**
 * usb_clock_config - configure USB controller clocks and reset the controller
 */
void usb_clock_config(void)
{
	/* Magic clock initialization numbers, nobody knows how they work... */
	write32(USB30_MASTER_CLK_CTL_REG, 0x10);
	write32(USB30_1_MASTER_CLK_CTL_REG, 0x10);
	write32(USB30_MASTER_CLK_MD, 0x500DF);
	write32(USB30_MASTER_CLK_NS, 0xE40942);
	write32(USB30_MOC_UTMI_CLK_MD, 0x100D7);
	write32(USB30_MOC_UTMI_CLK_NS, 0xD80942);
	write32(USB30_MOC_UTMI_CLK_CTL, 0x10);
	write32(USB30_1_MOC_UTMI_CLK_CTL, 0x10);

	write32(USB30_RESET,
		1 << 5 |		/* assert port2 HS PHY async reset */
		1 << 4 |		/* assert master async reset */
		1 << 3 |		/* assert sleep async reset */
		1 << 2 |		/* assert MOC UTMI async reset */
		1 << 1 |		/* assert power-on async reset */
		1 << 0);		/* assert PHY async reset */
	udelay(5);
	write32(USB30_RESET, 0);	/* deassert all USB resets again */
}
