/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <stdint.h>
#include <soc/clocks.h>
#include <console/console.h>

#define GPIO_BIT_EN_ADDR(bank)		(0xB8101C00 + 0x200 + (0x24 * (bank)))

/* MFIO definitions for UART0/1 */
#define UART1_RXD_MFIO			59
#define UART1_TXD_MFIO			60
#define UART0_RXD_MFIO			55
#define UART0_TXD_MFIO			56
#define UART0_RTS_MFIO			57
#define UART0_CTS_MFIO			58

/* MFIO definitions for SPIM */
#define SPIM1_D0_TXD_MFIO		5
#define SPIM1_D1_RXD_MFIO		4
#define SPIM1_MCLK_MFIO			3
#define SPIM1_D2_MFIO			6
#define SPIM1_D3_MFIO			7
#define SPIM1_CS0_MFIO			0

static void uart1_mfio_setup(void)
{
	u32 reg, mfio_mask;

	/*
	 * Disable GPIO for UART1 MFIOs
	 * All UART MFIOs have MFIO/16 = 3, therefore we use GPIO pad 3
	 * This is the primary function (0) of these MFIOs and therfore there
	 * is no need to set up a function number in the corresponding
	 * function select register.
	 */
	reg = read32(GPIO_BIT_EN_ADDR(3));
	mfio_mask =  1 << (UART1_RXD_MFIO % 16);
	mfio_mask |= 1 << (UART1_TXD_MFIO % 16);
	/* Clear relevant bits */
	reg &= ~mfio_mask;
	/*
	 * Set corresponding bits in the upper half word
	 * in order to be able to modify the chosen pins
	 */
	reg |= mfio_mask << 16;
	write32(GPIO_BIT_EN_ADDR(3), reg);
}

static void spim1_mfio_setup(void)
{
	u32 reg, mfio_mask;
	/*
	 * Disable GPIO for SPIM1 MFIOs
	 * All SPFI1 MFIOs have MFIO/16 = 0, therefore we use GPIO pad 0
	 * This is the primary function (0) of these MFIOs and therfore there
	 * is no need to set up a function number in the corresponding
	 * function select register.
	 */
	reg = read32(GPIO_BIT_EN_ADDR(0));

	/* Disable GPIO for UART0 MFIOs */
	mfio_mask = 1 << (SPIM1_D0_TXD_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_D1_RXD_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_MCLK_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_D2_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_D3_MFIO % 16);

	/* TODO: for the moment it only sets up CS0 (NOR) */
	/* There is no need for other CS lines in Coreboot */
	mfio_mask |= 1 << (SPIM1_CS0_MFIO % 16);

	/* Clear relevant bits */
	reg &= ~mfio_mask;
	/*
	 * Set corresponding bits in the upper half word
	 * in order to be able to modify the chosen pins
	 */
	reg |= mfio_mask << 16;
	write32(GPIO_BIT_EN_ADDR(0), reg);
}

static int init_clocks(void)
{
	int ret;

	/*
	 * Set up dividers for peripherals before setting up PLLs
	 * in order to not over-clock them when enabling PLLs
	*/

	/* System PLL divided by 2 -> 400 MHz */
	/* The same frequency will be the input frequency for the SPFI block */
	system_clk_setup(1);
	/* System clock divided by 8 -> 50 MHz */
	ret = usb_clk_setup(7, 2, 7);
	if (ret != CLOCKS_OK)
		return ret;
	/* System PLL divided by 7 divided by 62 -> 1.8433 Mhz */
	uart1_clk_setup(6, 61);

	/* Setup system PLL at 800 MHz */
	ret = sys_pll_setup(2, 1);
	if (ret != CLOCKS_OK)
		return ret;
	/* Setup MIPS PLL at 550 MHz */
	ret = mips_pll_setup(2, 1, 13, 275);
	if (ret != CLOCKS_OK)
		return ret;
	return CLOCKS_OK;
}

static void bootblock_mainboard_init(void)
{
	if (!init_clocks()) {
		/* Disable GPIO on the peripheral lines */
		uart1_mfio_setup();
		printk(BIOS_INFO, "Urara board: UART initialized!\n");

		spim1_mfio_setup();
	}

}
