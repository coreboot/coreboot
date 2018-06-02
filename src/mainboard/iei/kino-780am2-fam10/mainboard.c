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
#include "southbridge/amd/sb700/sb700.h"
#include "southbridge/amd/sb700/smbus.h"
#include "southbridge/amd/rs780/rs780.h"

void set_pcie_dereset(void);
void set_pcie_reset(void);
/* TODO - Need to find GPIO for PCIE slot.
 * Kino uses GPIO ? as PCIe slot reset, GPIO? as GFX slot reset. We need to
 * pull it up before training the slot.
 ***/
void set_pcie_dereset()
{
	/* PCIE slot not yet supported.*/
}

void set_pcie_reset()
{
	/* PCIE slot not yet supported.*/
}

int is_dev3_present(void)
{
	return 0;
}

/*************************************************
* enable the dedicated function in kino board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard Kino Enable. dev=0x%p\n", dev);

	set_pcie_dereset();
	/* get_ide_dma66(); */
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
