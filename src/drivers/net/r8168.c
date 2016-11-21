/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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

/*
 * This driver forces the 10ec:8168 device to reset so that it goes
 * into a proper power state, also programs a default MAC address
 * so that if the EEPROM/efuse is unconfigured it still has a default MAC.
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <delay.h>
#include <console/console.h>

#define NIC_TIMEOUT		1000

#define CMD_REG			0x37
#define  CMD_REG_RESET		0x10

#define CFG_9346		0x50
#define  CFG_9346_LOCK		0x00
#define  CFG_9346_UNLOCK	0xc0

static void r8168_init(struct device *dev)
{
	u32 i;
	const u8 mac[6] = { 0x00, 0xe0, 0x4c, 0x00, 0xc0, 0xb0 };

	/* Get the resource of the NIC mmio */
	struct resource *nic_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	u16 nic_port = (u16)nic_res->base;

	/* Ensble but do not set bus master. That's dangerous on a NIC. */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Reset NIC */
	printk(BIOS_DEBUG, "r8168: Resetting NIC...");
	outb(CMD_REG_RESET, nic_port + CMD_REG);

	i = 0;
	/* Poll for reset, with 1s timeout */
	while (i < NIC_TIMEOUT && (inb(nic_port + CMD_REG) & CMD_REG_RESET)) {
		udelay(1000);
		if (++i >= NIC_TIMEOUT)
			printk(BIOS_DEBUG, "timeout waiting for nic to reset\n");
	}
	if (i < NIC_TIMEOUT)
		printk(BIOS_DEBUG, "done\n");

	/* Unlock config regs */
	outb(CFG_9346_UNLOCK, nic_port + CFG_9346);

	/* Set MAC address 00:e0:4c:00:c0:b0
	 * NB: only 4-byte write accesses allowed
	 */
	outl(mac[4] | mac[5] << 8, nic_port + 4);
	inl(nic_port + 4);

	outl(mac[0] | mac[1] << 8 | mac[2] << 16 | mac[3] << 24, nic_port);
	inl(nic_port);

	/* Lock config regs */
	outb(CFG_9346_LOCK, nic_port + CFG_9346);
}

static struct device_operations r8168_ops  = {
       .read_resources   = pci_dev_read_resources,
       .set_resources    = pci_dev_set_resources,
       .enable_resources = pci_dev_enable_resources,
       .init             = r8168_init,
       .scan_bus         = 0,
};

static const struct pci_driver r8168_driver __pci_driver = {
        .ops    = &r8168_ops,
        .vendor = 0x10ec,
        .device = 0x8168,
};
