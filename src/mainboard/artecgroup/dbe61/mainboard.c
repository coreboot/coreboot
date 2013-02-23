/*
* This file is part of the coreboot project.
*
* Copyright (C) 2007 Advanced Micro Devices
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
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
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include "southbridge/amd/cs5536/cs5536.h"

static void init_gpio(void)
{
	msr_t msr;
	printk(BIOS_DEBUG, "Checking GPIO module...\n");

	msr = rdmsr(MDD_LBAR_GPIO);
	printk(BIOS_DEBUG, "DIVIL_LBAR_GPIO set to 0x%08x 0x%08x\n", msr.hi, msr.lo);
}

static void init(struct device *dev)
{
	// BOARD-SPECIFIC INIT
	printk(BIOS_DEBUG, "ARTECGROUP DBE61 ENTER %s\n", __func__);

	init_gpio();

	printk(BIOS_DEBUG, "ARTECGROUP DBE61 EXIT %s\n", __func__);
}

static void mainboard_enable(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
        .enable_dev = mainboard_enable,
};
