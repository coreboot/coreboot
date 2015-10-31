/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "hudson.h"
#include <southbridge/amd/common/amd_pci_util.h>
#include <bootstate.h>

static void pci_init(struct device *dev)
{
}

/*
 * Update the PCI devices with a valid IRQ number
 * that is set in the mainboard PCI_IRQ structures.
 */
static void set_pci_irqs(void *unused)
{
	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();

	/* Write IRQs for all devicetree enabled devices */
	write_pci_cfg_irqs();
}

/*
 * Hook this function into the PCI state machine
 * on entry into BS_DEV_ENABLE.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, set_pci_irqs, NULL);

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = pci_init,
	.scan_bus = pci_scan_bridge,
	.reset_bus = pci_bus_reset,
	.ops_pci = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_ATI_SB900_PCI,
};
