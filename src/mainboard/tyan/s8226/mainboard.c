/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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
#include <boot/tables.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <NbPlatform.h>
#include "chip.h"

void set_pcie_dereset(void *nbconfig);
void set_pcie_reset(void *nbconfig);

/**
 *
 */
void set_pcie_reset(void *nbconfig)
{
}

/**
 * Mainboard specific RD890 CIMx callback
 * Release Resets to PCIe Links
 * For Both SR56X0 chips, PCIE_RESET_GPIO1 to reset pcie
 */
void set_pcie_dereset(void *nbconfig)
{
	//u32 nb_dev = MAKE_SBDFO(0, 0x0, 0x0, 0x0, 0x0);
	u32 i;
	u32 val;
	u32 nb_addr;

	val = 0x00000007UL;
	AMD_NB_CONFIG_BLOCK *pConfig = (AMD_NB_CONFIG_BLOCK*)nbconfig;
	for (i = 0; i < MAX_NB_COUNT; i ++) {
		nb_addr = pConfig->Northbridges[i].NbPciAddress.AddressValue | NB_HTIU_INDEX;
		LibNbPciIndexRMW(nb_addr,
				NB_HTIU_REGA8,
				AccessS3SaveWidth32,
				~val,
				val,
				 &(pConfig->Northbridges[i]));
	}
}


/*************************************************
 * enable the dedicated function in s8226 board.
 *************************************************/
static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
}

struct chip_operations mainboard_ops = {
		.enable_dev = mainboard_enable,
};
