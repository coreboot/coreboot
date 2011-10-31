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
#include <boot/tables.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include "southbridge/amd/sr5650/cmn.h"
#include "chip.h"

void set_pcie_dereset(void);
void set_pcie_reset(void);

/**
 *
 */
void set_pcie_reset(void)
{
}

/**
 * Release Resets to PCIe Links
 * PCIE_RESET_GPIO1,2,4,5
 */
void set_pcie_dereset(void)
{
	device_t pcie_core_dev;

	pcie_core_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	set_htiu_enable_bits(pcie_core_dev, 0xA8, 0x07000707, 0x07000707);
	set_htiu_enable_bits(pcie_core_dev, 0xA9, 0x00000007, 0x00000007);
}


/*************************************************
* enable the dedicated function in h8qgi board.
*************************************************/
static void h8qgi_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
}

#if (CONFIG_HAVE_MAINBOARD_RESOURCES == 1)
int add_mainboard_resources(struct lb_memory *mem)
{
	return 0;
}
#endif

struct chip_operations mainboard_ops = {
	CHIP_NAME(CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER " Mainboard")
	.enable_dev = h8qgi_enable,
};
