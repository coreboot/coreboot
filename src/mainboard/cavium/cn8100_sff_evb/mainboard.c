/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-2018 Facebook, Inc.
 * Copyright 2003-2017 Cavium Inc. (support@cavium.com)
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
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <device/device.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-twsi.h>
#include <soc/twsi.h>
#include <soc/gpio.h>
#include <delay.h>
#include <soc/uart.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/gpio.h>
#include <soc/timer.h>
#include <soc/cpu.h>
#include <soc/sdram.h>

static void mainboard_print_info(void)
{
	printk(BIOS_INFO, "MB: trusted boot    : %s\n",
	       gpio_strap_value(10) ? "yes" : "no");

	const size_t boot_method = gpio_strap_value(0) |
		(gpio_strap_value(1) << 1) |
		(gpio_strap_value(2) << 2) |
		(gpio_strap_value(3) << 3);

	printk(BIOS_INFO, "MB: boot method     : ");
	switch (boot_method) {
	case 0x2:
	case 0x3:
		printk(BIOS_INFO, "EMMC\n");
		break;
	case 0x5:
	case 0x6:
		printk(BIOS_INFO, "SPI\n");
		break;
	case 0x8:
		printk(BIOS_INFO, "REMOTE\n");
		break;
	case 0xc:
	case 0xd:
		printk(BIOS_INFO, "PCIe\n");
		break;
	default:
		printk(BIOS_INFO, "unknown\n");
	}

	printk(BIOS_INFO, "MB: REFclk          : %llu MHz\n",
	       thunderx_get_ref_clock() / 1000000ULL);

	printk(BIOS_INFO, "MB: IOclk           : %llu MHz\n",
	       thunderx_get_io_clock() / 1000000ULL);

	printk(BIOS_INFO, "MB: COREclk         : %llu MHz\n",
	       thunderx_get_core_clock() / 1000000ULL);

	printk(BIOS_INFO, "MB: #CPU cores      : %zu\n",
	       cpu_get_num_cores());

	printk(BIOS_INFO, "MB: RAM             : %zu MiB\n",
		sdram_size_mb());
}

extern const struct bdk_devicetree_key_value devtree[];

static void mainboard_init(struct device *dev)
{
	size_t i;

	/* Init UARTs */
	for (i = 0; i < 4; i++) {
		if (!uart_is_enabled(i))
			uart_setup(i, 0);
	}

	/* Init timer */
	soc_timer_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;

	bdk_config_set_fdt(devtree);

	/*
	 * Adapted from Cavium's devicetree TWSI-WRITE:
	 * Init board-specific I2C hardware:
	 */
	twsi_init(0, I2C_SPEED_STANDARD);

	/* Initialize IO expander U6 to power-up defaults */
	/* float all pins 0.0-0.7 */
	bdk_twsix_write_ia(0,0,0x21,6,1,1,0xff);
	/* float all pins 1.0-1.7 */
	bdk_twsix_write_ia(0,0,0x21,7,1,1,0xff);
	/* 0.x: all outputs low, but disabled */
	bdk_twsix_write_ia(0,0,0x21,2,1,1,0x00);
	/* 1.x: all outputs low, but disabled */
	bdk_twsix_write_ia(0,0,0x21,3,1,1,0x00);
	/* 0.x: no polarity inversion */
	bdk_twsix_write_ia(0,0,0x21,4,1,1,0x00);
	/* 1.x: no polarity inversion */
	bdk_twsix_write_ia(0,0,0x21,5,1,1,0x00);
	/* Initialize IO expander U89 to power-up defaults */
	/* float all pins 0.0-0.7 */
	bdk_twsix_write_ia(0,0,0x22,6,1,1,0xff);
	/* float all pins 1.0-1.7 */
	bdk_twsix_write_ia(0,0,0x22,7,1,1,0xff);
	/* 0.x: all outputs low, but disabled */
	bdk_twsix_write_ia(0,0,0x22,2,1,1,0x00);
	/* 1.x: all outputs low, but disabled */
	bdk_twsix_write_ia(0,0,0x22,3,1,1,0x00);
	/* 0.x: no polarity inversion */
	bdk_twsix_write_ia(0,0,0x22,4,1,1,0x00);
	/* 1.x: no polarity inversion */
	bdk_twsix_write_ia(0,0,0x22,5,1,1,0x00);
	/* set outputs SLIC_RESET_L=0 and SPI_SEL=0 */
	bdk_twsix_write_ia(0,0,0x21,6,1,1,0xee); /* 0.0 & 0.4 are outputs */

	/* Select channel-0 in PCA9546A to enable SFI */
	bdk_twsix_write_ia(0, 0, 0x70, 0, 1, 1, 0x7);
	mdelay(10);
	/* Configure I2C-GPIO expander I/O directions */
	bdk_twsix_write_ia(0, 0, 0x22, 6, 1, 1, 0x07);
	mdelay(10);
	/* Configure I2C-GPIO expander I/O directions */
	bdk_twsix_write_ia(0, 0, 0x22, 7, 1, 1, 0x38);
	mdelay(10);
	/* Turn on SFP+ Transmitters */
	bdk_twsix_write_ia(0, 0, 0x22, 2, 1, 1, 0x0);
	mdelay(10);
	/* Set VSC7224 to I2C mode */
	bdk_twsix_write_ia(0, 0, 0x22, 3, 1, 1, 0x0);
	mdelay(10);
	/* Assert VSC7224 reset*/
	bdk_twsix_write_ia(0, 0, 0x22, 2, 1, 1, 0x80);
	mdelay(50);
	/* Deassert VSC7224 reset*/
	bdk_twsix_write_ia(0, 0, 0x22, 2, 1, 1, 0x0);
	mdelay(50);
	/* Page select FSYNC0 (0x30) */
	bdk_twsix_write_ia(0, 0, 0x14, 0x7f, 2, 1, 0x0030);
	mdelay(10);
	/* Set FSYNC0 for 10.3125Gbps  See Table 3 */
	bdk_twsix_write_ia(0, 0, 0x14, 0x80, 2, 1, 0x2841);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x81, 2, 1, 0x0008);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x82, 2, 1, 0x7a00);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x83, 2, 1, 0x000f);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x84, 2, 1, 0x9c18);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x85, 2, 1, 0x0);
	mdelay(10);

	/* All channels Rx settings set equally */
	bdk_twsix_write_ia(0, 0, 0x14, 0x7f, 2, 1, 0x0050);
	mdelay(10);
	/* Shrink EQ_BUFF */
	bdk_twsix_write_ia(0, 0, 0x14, 0x82, 2, 1, 0x0014);
	mdelay(10);
	/* Select min DFE Delay (DFE_DELAY) */
	bdk_twsix_write_ia(0, 0, 0x14, 0x90, 2, 1, 0x5585);
	mdelay(10);
	/* Set DFE 1-3 limit (DXMAX) = 32dec, AP Max limit = 127 decimal */
	bdk_twsix_write_ia(0, 0, 0x14, 0x92, 2, 1, 0x207f);
	mdelay(10);
	/* Set AP Min limit = 32 decimal */
	bdk_twsix_write_ia(0, 0, 0x14, 0x93, 2, 1, 0x2000);
	mdelay(10);
	/* Set DFE Averaging to the slowest (DFE_AVG) */
	bdk_twsix_write_ia(0, 0, 0x14, 0x94, 2, 1, 0x0031);
	mdelay(10);
	/* Set Inductor Bypass OD_IND_BYP = 0 & fastest Rise/Fall */
	bdk_twsix_write_ia(0, 0, 0x14, 0x9c, 2, 1, 0x0000);
	mdelay(10);
	/* Setting DFE Boost = none. Must set for rev C
	 * (if DFE in adapt mode) */
	bdk_twsix_write_ia(0, 0, 0x14, 0xaa, 2, 1, 0x0888);
	mdelay(10);
	/* Setting EQ Min/Max = 8/72 */
	bdk_twsix_write_ia(0, 0, 0x14, 0xa8, 2, 1, 0x2408);
	mdelay(10);
	/* Setting EQVGA = 96, when in EQVGA manual mode */
	bdk_twsix_write_ia(0, 0, 0x14, 0xa9, 2, 1, 0x0060);
	mdelay(10);
	/* Setting SW_BFOCM, bits 15:14 to 01 */
	bdk_twsix_write_ia(0, 0, 0x14, 0x87, 2, 1, 0x4021);
	mdelay(10);
	/* Turn off adaptive input equalization and VGA adaptive algorithm
	 * control */
	bdk_twsix_write_ia(0, 0, 0x14, 0x89, 2, 1, 0x7313);
	mdelay(10);
	/* Turn on adaptive input equalization and VGA adaptive algorithm
	 * control */
	bdk_twsix_write_ia(0, 0, 0x14, 0x89, 2, 1, 0x7f13);
	mdelay(10);

	/* TAP settings for each channel 0-3 */
	/* Ch-0 Tx */
	bdk_twsix_write_ia(0, 0, 0x14, 0x7f, 2, 1, 0x0000);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x99, 2, 1, 0x001f);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x9a, 2, 1, 0x000f);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x9b, 2, 1, 0x0004);
	mdelay(10);

	/* Ch-1 Rx */
	bdk_twsix_write_ia(0, 0, 0x14, 0x7f, 2, 1, 0x0001);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x97, 2, 1, 0x1400);
	mdelay(10);
	/* Transmitter Output polarity Inverted (Unfortunately,
	 * Rx polarity lines are wrongly inverted on board */
	bdk_twsix_write_ia(0, 0, 0x14, 0x97, 2, 1, 0x4000);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x99, 2, 1, 0x000f);
	mdelay(10);

	/* Ch-2 Tx */
	bdk_twsix_write_ia(0, 0, 0x14, 0x7f, 2, 1, 0x0002);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x99, 2, 1, 0x001f);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x9a, 2, 1, 0x000f);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x9b, 2, 1, 0x0004);
	mdelay(10);

	/* Ch-3 Rx */
	bdk_twsix_write_ia(0, 0, 0x14, 0x7f, 2, 1, 0x0003);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x97, 2, 1, 0x1400);
	mdelay(10);
	/* Transmitter Output polarity Inverted (Unfortunately,
	 * Rx polarity lines are wrongly inverted on board */
	bdk_twsix_write_ia(0, 0, 0x14, 0x97, 2, 1, 0x4000);
	mdelay(10);
	bdk_twsix_write_ia(0, 0, 0x14, 0x99, 2, 1, 0x000f);
	mdelay(10);

	/**
	 * The following hardware magically starts working after toggling
	 * GPIO_10_PHY_RESET_L:
	 * * SATA PHY
	 * * GBE PHY
	 * * XFI PHY
	 * * MMC
	 */
	gpio_output(10, 0);
	udelay(100);
	gpio_output(10, 1);

	mainboard_print_info();
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
