/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>

/**********************************************
 * enable the dedicated function in mainboard.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
