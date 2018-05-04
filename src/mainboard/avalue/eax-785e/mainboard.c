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
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <southbridge/amd/common/amd_defs.h>
#include <device/pci_def.h>

u8 is_dev3_present(void);
void set_pcie_dereset(void);
void set_pcie_reset(void);
void enable_int_gfx(void);

/* GPIO6. */
void enable_int_gfx(void)
{
	volatile u8 *gpio_reg;

	/* make sure the Acpi MMIO(fed80000) is accessible */
        // FIXME: RWPMIO(SB_PMIOA_REG24, AccWidthUint8, ~(BIT0), BIT0);

	gpio_reg = (volatile u8 *)AMD_SB_ACPI_MMIO_ADDR + 0xD00; /* IoMux Register */

	*(gpio_reg + 0x6) = 0x1; /* Int_vga_en */
	*(gpio_reg + 170) = 0x1; /* gpio_gate */

	gpio_reg = (volatile u8 *)AMD_SB_ACPI_MMIO_ADDR + 0x100; /* GPIO Registers */

	*(gpio_reg + 0x6) = 0x8;
	*(gpio_reg + 170) = 0x0;
}

void set_pcie_dereset()
{
}

void set_pcie_reset(void)
{
}

u8 is_dev3_present(void)
{
	return 1;
}


/*************************************************
* enable the dedicated function in EAX-785E board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	set_pcie_dereset();
	enable_int_gfx();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
