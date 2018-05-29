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
#include <arch/io.h>

#include <southbridge/amd/cimx/cimx_util.h>
#include "SBPLATFORM.h"

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* Power off unused clock pins of GPP PCIe devices */
	u8 *misc_mem_clk_cntrl = (u8 *)(ACPI_MMIO_BASE + MISC_BASE);
	/*
	 * GPP CLK0 connected to unpopulated mini PCIe slot
	 * GPP CLK1 connected to ethernet chip
	 */
	write8(misc_mem_clk_cntrl + 0, 0xFF);
	/* GPP CLK2 connected to the external USB3 controller */
	write8(misc_mem_clk_cntrl + 1, 0x0F);
	write8(misc_mem_clk_cntrl + 2, 0x00);
	write8(misc_mem_clk_cntrl + 3, 0x00);
	/* SLT_GFX_CLK connected to PCIe slot */
	write8(misc_mem_clk_cntrl + 4, 0xF0);

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
