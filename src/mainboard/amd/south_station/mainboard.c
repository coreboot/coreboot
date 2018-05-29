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

#include <console/console.h>
#include <delay.h>
#include <device/device.h>

#include <southbridge/amd/sb800/sb800.h>
#include "SBPLATFORM.h" 	/* Platfrom Specific Definitions */

/**
 * Southstation using SB GPIO 17/18 to control the Red/Green LED
 * These two LEDs can be used to show the OS booting status.
 */
static void southstation_led_init(void)
{
#define GPIO_FUNCTION	2  //GPIO function
#define SB_GPIO_REG17	17 //Red  Light
#define SB_GPIO_REG18	18 //Green Light

	/* multi-function pins switch to GPIO0-35 */
	RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEA, AccWidthUint8, ~BIT0, 1);

	/* select IOMux to function2, corresponds to GPIO */
	RWMEM(ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG17, AccWidthUint8, ~(BIT0 | BIT1), GPIO_FUNCTION);
	RWMEM(ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG18, AccWidthUint8, ~(BIT0 | BIT1), GPIO_FUNCTION);

	/* Lighting test */
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG17, AccWidthUint8, ~(0xFF), 0x08); //output high
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG18, AccWidthUint8, ~(0xFF), 0x08);
	mdelay(100);
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG17, AccWidthUint8, ~(0xFF), 0x48); //output low
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG18, AccWidthUint8, ~(0xFF), 0x48);
}


/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	southstation_led_init();

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
