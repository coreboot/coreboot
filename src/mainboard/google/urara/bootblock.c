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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <stdint.h>
#include <soc/clocks.h>
#include <assert.h>
#include <boardid.h>

#define PADS_FUNCTION_SELECT0_ADDR	(0xB8101C00 + 0xC0)

#define GPIO_BIT_EN_ADDR(bank)		(0xB8101C00 + 0x200 + (0x24 * (bank)))
#define PAD_DRIVE_STRENGTH_ADDR(bank)	(0xB8101C00 + 0x120 + (0x4 * (bank)))
#define MAX_NO_MFIOS				89
#define PAD_DRIVE_STRENGTH_LENGTH		2
#define PAD_DRIVE_STRENGTH_MASK			0x3

typedef enum {
	DRIVE_STRENGTH_2mA      = 0,
	DRIVE_STRENGTH_4mA      = 1,
	DRIVE_STRENGTH_8mA      = 2,
	DRIVE_STRENGTH_12mA     = 3
} drive_strength;

/* MFIO definitions for UART1 */
#define UART1_RXD_MFIO			59
#define UART1_TXD_MFIO			60

/* MFIO definitions for SPIM */
#define SPIM1_D0_TXD_MFIO		5
#define SPIM1_D1_RXD_MFIO		4
#define SPIM1_MCLK_MFIO			3
#define SPIM1_D2_MFIO			6
#define SPIM1_D3_MFIO			7
#define SPIM1_CS0_MFIO			0

/* MFIO definitions for I2C */
#define I2C_DATA_MFIO(i)		(28 + (2*(i)))
#define I2C_CLK_MFIO(i)			(29 + (2*(i)))
#define I2C_DATA_FUNCTION_OFFSET(i)	(20 + (2*(i)))
#define I2C_CLK_FUNCTION_OFFSET(i)	(21 + (2*(i)))
#define I2C_DATA_FUNCTION_MASK		0x1
#define I2C_CLK_FUNCTION_MASK		0x1

static void pad_drive_strength(u32 pad, drive_strength strength)
{
	u32 reg, drive_strength_shift;

	assert(pad <= MAX_NO_MFIOS);
	assert(!(strength & ~(PAD_DRIVE_STRENGTH_MASK)));

	/* Set drive strength value */
	drive_strength_shift = (pad % 16) * PAD_DRIVE_STRENGTH_LENGTH;
	reg = read32(PAD_DRIVE_STRENGTH_ADDR(pad / 16));
	reg &= ~(PAD_DRIVE_STRENGTH_MASK << drive_strength_shift);
	reg |= strength << drive_strength_shift;
	write32(PAD_DRIVE_STRENGTH_ADDR(pad / 16), reg);
}

static void uart1_mfio_setup(void)
{
	u32 reg, mfio_mask;

	/*
	 * Disable GPIO for UART1 MFIOs
	 * All UART MFIOs have MFIO/16 = 3, therefore we use GPIO pad 3
	 * This is the only function (0) of these MFIOs and therfore there
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
	 * This is the only function (0) of these MFIOs and therfore there
	 * is no need to set up a function number in the corresponding
	 * function select register.
	 */
	reg = read32(GPIO_BIT_EN_ADDR(0));

	/* Disable GPIO for SPIM1 MFIOs */
	mfio_mask = 1 << (SPIM1_D0_TXD_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_D1_RXD_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_MCLK_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_D2_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_D3_MFIO % 16);
	mfio_mask |= 1 << (SPIM1_CS0_MFIO % 16);

	/* Clear relevant bits */
	reg &= ~mfio_mask;
	/*
	 * Set corresponding bits in the upper half word
	 * in order to be able to modify the chosen pins
	 */
	reg |= mfio_mask << 16;
	write32(GPIO_BIT_EN_ADDR(0), reg);

	/* Set drive strength to maximum for these MFIOs */
	pad_drive_strength(SPIM1_CS0_MFIO, DRIVE_STRENGTH_12mA);
	pad_drive_strength(SPIM1_D1_RXD_MFIO, DRIVE_STRENGTH_12mA);
	pad_drive_strength(SPIM1_D0_TXD_MFIO, DRIVE_STRENGTH_12mA);
	pad_drive_strength(SPIM1_D2_MFIO, DRIVE_STRENGTH_12mA);
	pad_drive_strength(SPIM1_D3_MFIO, DRIVE_STRENGTH_12mA);
	pad_drive_strength(SPIM1_MCLK_MFIO, DRIVE_STRENGTH_12mA);
}

static void i2c_mfio_setup(int interface)
{
	u32 reg, mfio_mask;

	assert(interface < 4);
	/*
	 * Disable GPIO for I2C MFIOs
	 */
	reg = read32(GPIO_BIT_EN_ADDR(I2C_DATA_MFIO(interface) / 16));
	mfio_mask =  1 << (I2C_DATA_MFIO(interface) % 16);
	mfio_mask |= 1 << (I2C_CLK_MFIO(interface) % 16);
	/* Clear relevant bits */
	reg &= ~mfio_mask;
	/*
	 * Set corresponding bits in the upper half word
	 * in order to be able to modify the chosen pins
	 */
	reg |= mfio_mask << 16;
	write32(GPIO_BIT_EN_ADDR(I2C_DATA_MFIO(interface) / 16), reg);

	/* for I2C0 and I2C1:
	 * Set bits to 0 (clear) which is the primary function
	 * for these MFIOs; those bits will all be set to 1 by
	 * default.
	 * There is no need to do that for I2C2 and I2C3
	 */
	if (interface > 1)
		return;
	reg = read32(PADS_FUNCTION_SELECT0_ADDR);
	reg &= ~(I2C_DATA_FUNCTION_MASK <<
		I2C_DATA_FUNCTION_OFFSET(interface));
	reg &= ~(I2C_CLK_FUNCTION_MASK <<
		I2C_CLK_FUNCTION_OFFSET(interface));
	write32(PADS_FUNCTION_SELECT0_ADDR, reg);
}

static void bootblock_mainboard_init(void)
{
	int ret;

	/* System PLL divided by 2 -> 400 MHz */
	/* The same frequency will be the input frequency for the SPFI block */
	system_clk_setup(1);

	/* MIPS CPU dividers: division by 1 -> 546 MHz
	 * This is set up as we cannot make any assumption about
	 * the values set or not by the boot ROM code */
	mips_clk_setup(0, 0);

	/* Setup system PLL at 800 MHz */
	ret = sys_pll_setup(2, 1);
	if (ret != CLOCKS_OK)
		return;
	/* Setup MIPS PLL at 546 MHz */
	ret = mips_pll_setup(2, 1, 1, 21);
	if (ret != CLOCKS_OK)
		return;

	/* Setup SPIM1 MFIOs */
	spim1_mfio_setup();
	/* Setup UART1 clock and MFIOs
	 * System PLL divided by 7 divided by 62 -> 1.8433 Mhz
	 */
	uart1_clk_setup(6, 61);
	uart1_mfio_setup();
}


static int init_extra_hardware(void)
{
	const struct board_hw *hardware;

	/* Obtain information about current board */
	hardware = board_get_hw();
	if (!hardware) {
		printk(BIOS_ERR, "%s: Invalid hardware information.\n",
			__func__);
		return -1;
	}

	/* Setup USB clock
	 * System clock divided by 8 -> 50 MHz
	 */
	if (usb_clk_setup(7, 2, 7) != CLOCKS_OK) {
		printk(BIOS_ERR, "%s: Failed to set up USB clock.\n",
			__func__);
		return -1;
	}

	/* Setup I2C clocks and MFIOs
	 * System PLL divided by 4 divided by 3 -> 33.33 MHz
	 */
	i2c_clk_setup(3, 2, hardware->i2c_interface);
	i2c_mfio_setup(hardware->i2c_interface);

	/* Ethernet clocks setup: ENET as clock source */
	eth_clk_setup(0, 7);
	/* ROM clock setup: system clock divided by 2 -> 200 MHz */
	/* Hash accelerator is driven from the ROM clock */
	rom_clk_setup(1);

	return 0;
}
