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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include "SBPLATFORM.h" 	/* Platfrom Specific Definitions */


void set_pcie_reset(void);
void set_pcie_dereset(void);

/**
 * TODO
 * SB CIMx callback
 */
void set_pcie_reset(void)
{
}

/**
 * TODO
 * mainboard specific SB CIMx callback
 */
void set_pcie_dereset(void)
{
}

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


/*************************************************
* enable the dedicated function in southstation board.
*************************************************/
static void southstation_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
	southstation_led_init();
}

struct chip_operations mainboard_ops = {
	CHIP_NAME(CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER " Mainboard")
	.enable_dev = southstation_enable,
};
