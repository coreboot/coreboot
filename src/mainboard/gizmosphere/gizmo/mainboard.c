/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include <southbridge/amd/sb800/sb800.h>
#include "SBPLATFORM.h"

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* enable GPP CLK0 thru CLK1 */
	/* disable GPP CLK2 thru SLT_GFX_CLK */
	u8 *misc_mem_clk_cntrl = (u8 *)(ACPI_MMIO_BASE + MISC_BASE);
	write8(misc_mem_clk_cntrl + 0, 0xFF);
	write8(misc_mem_clk_cntrl + 1, 0x00);
	write8(misc_mem_clk_cntrl + 2, 0x00);
	write8(misc_mem_clk_cntrl + 3, 0x00);
	write8(misc_mem_clk_cntrl + 4, 0x00);

	/*
	 * Force the onboard SATA port to GEN2 speed.
	 * The offboard SATA port can remain at GEN3.
	 */
	RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, 0xFB, 0x04);
}

static void mainboard_final(void *chip_info)
{
	device_t ahci_dev;
	uintptr_t ABAR;
	u8 *memptr;

	ahci_dev = dev_find_slot(0, PCI_DEVFN(0x11, 0));
	ABAR = pci_read_config32(ahci_dev, 0x24);
	ABAR &= 0xFFFFFC00;
	memptr = (u8 *)(ABAR + 0x100 + 0x80 + 0x2C); /* we're on the 2nd port */
	*memptr = 0x21; /* force to GEN2 and start re-negotiate */
	mdelay (1);
	*memptr = 0x20;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
