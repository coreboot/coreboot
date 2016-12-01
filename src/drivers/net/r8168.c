/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 * This driver resets the 10ec:8168 NIC then tries to read
 * "macaddress" string XX:XX:XX:XX:XX:XX from CBFS.
 * If no MAC is found, it programs a default MAC address in the device
 * so that if the EEPROM/efuse is unconfigured it still has a default MAC.
 */

#include <cbfs.h>
#include <string.h>
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

static u8 get_hex_digit(const u8 c)
{
	u8 ret = 0;

	ret = c - '0';
	if (ret > 0x09) {
		ret = c - 'A' + 0x0a;
		if (ret > 0x0f)
			ret = c - 'a' + 0x0a;
	}
	if (ret > 0x0f) {
		printk(BIOS_DEBUG, "Error: Invalid hex digit found: "
				   "%c - 0x%02x\n", (char)c, c);
		ret = 0;
	}
	return ret;
}

static void get_mac_address(u8 *macaddr, const u8 *strbuf)
{
	size_t offset = 0;
	int i;

	if ( (strbuf[2] != ':') || (strbuf[5] != ':') ||
	     (strbuf[8] != ':') || (strbuf[11] != ':') ||
	     (strbuf[14] != ':') ) {
		printk(BIOS_ERR, "r8168: ignore invalid MAC address in cbfs\n");
		return;
	}

	for (i = 0; i < 6; i++) {
		macaddr[i] = 0;
		macaddr[i] |= get_hex_digit(strbuf[offset]) << 4;
		macaddr[i] |= get_hex_digit(strbuf[offset + 1]);
		offset += 3;
	}
}

#define MACLEN 17

static void program_mac_address(struct device *dev, u16 io_base)
{
	struct cbfsf fh;
	uint32_t matchraw = CBFS_TYPE_RAW;
	u8 macstrbuf[MACLEN] = { 0 };
	int i = 0;
	u8 mac[6] = { 0x00, 0xe0, 0x4c, 0x00, 0xc0, 0xb0 };

	if (!cbfs_boot_locate(&fh, "macaddress", &matchraw)) {
		if (rdev_readat(&fh.data, macstrbuf, 0, MACLEN) == MACLEN)
			get_mac_address(mac, macstrbuf);
		else
			printk(BIOS_ERR, "r8168: Error reading MAC from CBFS\n");
	} else {
		printk(BIOS_ERR, "r8168: 'macaddress' not found in CBFS, "
				 "using default 00:e0:4c:00:c0:b0\n");
	}

	/* Reset NIC */
	printk(BIOS_DEBUG, "r8168: Resetting NIC...");
	outb(CMD_REG_RESET, io_base + CMD_REG);

	/* Poll for reset, with 1sec timeout */
	while (i < NIC_TIMEOUT && (inb(io_base + CMD_REG) & CMD_REG_RESET)) {
		udelay(1000);
		if (++i >= NIC_TIMEOUT)
			printk(BIOS_DEBUG, "timeout waiting for nic to reset\n");
	}
	if (i < NIC_TIMEOUT)
		printk(BIOS_DEBUG, "done\n");

	printk(BIOS_DEBUG, "r8168: Programming MAC Address...");

	/* Disable register protection */
	outb(CFG_9346_UNLOCK, io_base + CFG_9346);

	/* Set MAC address: only 4-byte write accesses allowed */
	outl(mac[4] | mac[5] << 8, io_base + 4);
	inl(io_base + 4);
	outl(mac[0] | mac[1] << 8 | mac[2] << 16 | mac[3] << 24,
		io_base);
	inl(io_base);
	/* Lock config regs */
	outb(CFG_9346_LOCK, io_base + CFG_9346);

	printk(BIOS_DEBUG, "done\n");
}

static void r8168_init(struct device *dev)
{
	/* Get the resource of the NIC mmio */
	struct resource *nic_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	u16 io_base = (u16)nic_res->base;

	/* Enable but do not set bus master */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Program MAC address based on CBFS "macaddress" containing
	 * a string AA:BB:CC:DD:EE:FF */
	if (io_base)
		program_mac_address(dev, io_base);
	else
		printk(BIOS_DEBUG, "r8168: Error cant find MMIO resource\n");
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
