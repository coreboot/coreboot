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
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "rs780.h"

/* for UMA internal graphics */
void avoid_lpc_dma_deadlock(struct device *nb_dev, struct device *sb_dev)
{
	struct device *cpu_f0;
	u8 reg;

	cpu_f0 = dev_find_slot(0, PCI_DEVFN(0x18, 0));
	set_nbcfg_enable_bits(cpu_f0, 0x68, 3 << 21, 1 << 21);

	reg = nbpcie_p_read_index(sb_dev, 0x10);
	reg |= 0x100;		/* bit9=1 */
	nbpcie_p_write_index(sb_dev, 0x10, reg);

	reg = nbpcie_p_read_index(nb_dev, 0x10);
	reg |= 0x100;		/* bit9=1 */
	nbpcie_p_write_index(nb_dev, 0x10, reg);

	/* Enable NP protocol over PCIE for memory-mapped writes targeting LPC
	 * Set this bit to avoid a deadlock condition. */
	reg = htiu_read_index(nb_dev, 0x6);
	reg |= 0x1000000;	/* bit26 */
	htiu_write_index(nb_dev, 0x6, reg);
}

static void pcie_init(struct device *dev)
{
	/* Enable pci error detecting */
	u32 dword;

	printk(BIOS_INFO, "pcie_init in rs780_ht.c\n");

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* System error enable */
	dword |= (1 << 30);	/* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

	/*
	 * 1 is APIC enable
	 * 18 is enable nb to accept A4 interrupt request from SB.
	 */
	dword = pci_read_config32(dev, 0x4C);
	dword |= 1 << 1 | 1 << 18;	/* Clear possible errors */
	pci_write_config32(dev, 0x4C, dword);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations ht_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = pcie_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver ht_driver __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_RS780_HT,
};
