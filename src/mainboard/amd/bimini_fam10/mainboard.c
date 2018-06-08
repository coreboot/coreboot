/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <southbridge/amd/common/amd_defs.h>
#include <southbridge/amd/sb800/sb800.h>
#include "southbridge/amd/rs780/rs780.h"

/* GPIO6. */
static void enable_int_gfx(void)
{
	u8 byte;

	volatile u8 *gpio_reg;

	pm_iowrite(0xEA, 0x01);	/* diable the PCIB */
	/* Disable Gec */
	byte = pm_ioread(0xF6);
	byte |= 1;
	pm_iowrite(0xF6, byte);
	/* make sure the fed80000 is accessible */
	byte = pm_ioread(0x24);
	byte |= 1;
	pm_iowrite(0x24, byte);

	gpio_reg = (volatile u8 *)AMD_SB_ACPI_MMIO_ADDR + 0xD00; /* IoMux Register */

	*(gpio_reg + 0x6) = 0x1; /* Int_vga_en */
	*(gpio_reg + 170) = 0x1; /* gpio_gate */

	gpio_reg = (volatile u8 *)AMD_SB_ACPI_MMIO_ADDR + 0x100; /* GPIO Registers */

	*(gpio_reg + 0x6) = 0x8;
	*(gpio_reg + 170) = 0x0;
}

/*
 * Bimini uses GPIO 6 as PCIe slot reset, GPIO4 as GFX slot reset. We need to
 * pull it up before training the slot.
 *
 * Old comment says: GPIO 50h to reset PCIe slot.
 ***/

int is_dev3_present(void)
{
	return 0;
}


/*************************************************
* enable the dedicated function in bimini board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard BIMINI Enable. dev=0x%p\n", dev);

	set_pcie_dereset();
	enable_int_gfx();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
