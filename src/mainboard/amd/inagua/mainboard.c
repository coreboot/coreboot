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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb800/sb800.h>
#include "SBPLATFORM.h" 	/* Platfrom Specific Definitions */

void broadcom_init(void);
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
 * mainboard specific SB CIMx callback
 */
void set_pcie_dereset(void)
{
	/**
	 * GPIO32 Pcie Device DeAssert for APU
	 * GPIO25 Pcie LAN,       APU GPP2
	 * GPIO02 MINIPCIE SLOT1, APU GPP3
	 * GPIO50 Pcie Device DeAssert for Hudson Southbridge
	 * GPIO05 Express Card,     SB  GPP0
	 * GPIO26 NEC USB3.0GPPUSB, SB  GPP1
	 * GPIO00 MINIPCIE SLOT2,   SB  GPP2
	 * GPIO05 Pcie X1 Slot,     SB  GPP3
	 */

	/* Multi-function pins switch to GPIO0-35, these pins are shared with
	 * PCI pins, make sure Husson PCI device is disabled.
	 */
	RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEA, AccWidthUint8, ~BIT0, 1);

	/* select IOMux to function1/2, corresponds to GPIO */
	RWMEM(ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG32, AccWidthUint8, ~(BIT0 | BIT1), 1);
	RWMEM(ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG50, AccWidthUint8, ~(BIT0 | BIT1), 2);


	/* output low */
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG32, AccWidthUint8, ~(0xFF), 0x48);
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG50, AccWidthUint8, ~(0xFF), 0x48);
}


/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* Inagua mainboard specific setting */
	set_pcie_dereset();

	/*
	 * Initialize ASF registers to an arbitrary address because someone
	 * long ago set things up this way inside the SPD read code.  The
	 * SPD read code has been made generic and moved out of the board
	 * directory, so the ASF init is being done here.
	 */
	pm_iowrite(0x29, 0x80);
	pm_iowrite(0x28, 0x61);

	/* Upload AMD A55E GbE 'NV'RAM contents.  Still untested on Inagua.
	 * After anyone can confirm it works please uncomment the call. */
	//broadcom_init();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
