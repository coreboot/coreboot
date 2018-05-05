/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <stdlib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb800/sb800.h>
#include <arch/acpi.h>
#include "SBPLATFORM.h"
#include "OEM.h" /* SMBUS0_BASE_ADDRESS */
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include "sema.h"

/* Init SIO GPIOs. */
#define SIO_RUNTIME_BASE 0x0E00
static const u16 sio_init_table[] = { // hi = offset, lo = value
	0x4BA0, // GP1x: COM1/2 control   = RS232, no term, max 115200
	0x2300, // GP10: COM1 termination = push/pull output
	0x2400, // GP11: COM2 termination = push/pull output
	0x2500, // GP12: COM1 RS485 mode  = push/pull output
	0x2600, // GP13: COM2 RS485 mode  = push/pull output
	0x2700, // GP14: COM1 speed A     = push/pull output
	0x2900, // GP15: COM1 speed B     = push/pull output
	0x2A00, // GP16: COM2 speed A     = push/pull output
	0x2B00, // GP17: COM2 speed B     = push/pull output

	0x3904, // GP36                   = KBDRST# function

	0x4E74, // GP4x: Ethernet enable  = on
	0x6E84, // GP44: Ethernet enable  = open drain output

	// GP5x = COM2 function instead of GPIO
	0x3F05, 0x4005, 0x4105, 0x4204, 0x4305, 0x4404, 0x4505, 0x4604,

	0x470C, // GP60                   = WDT function
	0x5E00, // LED2: Live LED         = off
	0x4884, // GP61: Live LED         = LED2 function

	0x5038, // GP6x: USB power        = 3x on
	0x5580, // GP63: USB power 0/1    = open drain output
	0x5680, // GP64: USB power 2/3    = open drain output
	0x5780, // GP65: USB power 4/5    = open drain output
};

static void init(struct device *dev)
{
	volatile u8 *spi_base;	// base addr of Hudson's SPI host controller
	int i;
	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " ENTER %s\n", __func__);

	/* Init Hudson GPIOs. */
	printk(BIOS_DEBUG, "Init FCH GPIOs @ 0x%08x\n", ACPI_MMIO_BASE+GPIO_BASE);
	FCH_IOMUX( 50) = 2;    // GPIO50: FCH_ARST#_GATE resets stuck PCIe devices
	FCH_GPIO ( 50) = 0xC0; // = output set to 1 as it's never needed
	FCH_IOMUX(197) = 2;    // GPIO197: BIOS_DEFAULTS# = input (int. PU)
	FCH_IOMUX( 56) = 1;    // GPIO58-56: REV_ID2-0
	FCH_GPIO ( 56) = 0x28; // = inputs, disable int. pull-ups
	FCH_IOMUX( 57) = 1;
	FCH_GPIO ( 57) = 0x28;
	FCH_IOMUX( 58) = 1;
	FCH_GPIO ( 58) = 0x28;
	FCH_IOMUX( 96) = 1;    // "Gpio96": GEVENT0# signal on X2 connector (int. PU)
	FCH_IOMUX( 52) = 1;    // GPIO52,61,62,187-192 free to use on X2 connector
	FCH_IOMUX( 61) = 2;    // default to inputs with int. PU
	FCH_IOMUX( 62) = 2;
	FCH_IOMUX(187) = 2;
	FCH_IOMUX(188) = 2;
	FCH_IOMUX(189) = 1;
	FCH_IOMUX(190) = 1;
	FCH_IOMUX(191) = 1;
	FCH_IOMUX(192) = 1;
	if (!fch_gpio_state(197)) // just in case anyone cares
		printk(BIOS_INFO, "BIOS_DEFAULTS jumper is present.\n");
	printk(BIOS_INFO, "Board revision ID: %u\n",
	       fch_gpio_state(58)<<2 | fch_gpio_state(57)<<1 | fch_gpio_state(56));

	/* Init SIO GPIOs. */
	printk(BIOS_DEBUG, "Init SIO GPIOs @ 0x%04x\n", SIO_RUNTIME_BASE);
	for (i = 0; i < ARRAY_SIZE(sio_init_table); i++) {
		u16 val = sio_init_table[i];
		outb((u8)val, SIO_RUNTIME_BASE + (val >> 8));
	}

	/* Lower SPI speed from default 66 to 22 MHz for SST 25VF032B */
	spi_base = (u8*)((uintptr_t)pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x14, 3)), 0xA0) & 0xFFFFFFE0);
	spi_base[0x0D] = (spi_base[0x0D] & ~0x30) | 0x20; // NormSpeed in SPI_Cntrl1 register

	/* Notify the SMC we're alive and kicking, or after a while it will
	 * effect a power cycle and switch to the alternate BIOS chip.
	 * Should be done as late as possible.
	 * Failure here does not matter if watchdog was already disabled,
	 * by configuration or previous boot, so ignore return value.
	 */
	sema_send_alive();

	printk(BIOS_DEBUG, CONFIG_MAINBOARD_PART_NUMBER " EXIT %s\n", __func__);
}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	dev->ops->init = init;

	/* enable GPP CLK0 */
	/* disable GPP CLK1 thru SLT_GFX_CLK */
	u8 *misc_mem_clk_cntrl = (u8 *)(ACPI_MMIO_BASE + MISC_BASE);
	write8(misc_mem_clk_cntrl + 0, 0x0F);
	write8(misc_mem_clk_cntrl + 1, 0x00);
	write8(misc_mem_clk_cntrl + 2, 0x00);
	write8(misc_mem_clk_cntrl + 3, 0x00);
	write8(misc_mem_clk_cntrl + 4, 0x00);

	/*
	 * Initialize ASF registers to an arbitrary address because someone
	 * long ago set things up this way inside the SPD read code.  The
	 * SPD read code has been made generic and moved out of the board
	 * directory, so the ASF init is being done here.
	 */
	pm_iowrite(0x29, 0x80);
	pm_iowrite(0x28, 0x61);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
