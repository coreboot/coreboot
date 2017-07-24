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
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <delay.h>
#include <fmap.h>

#define NIC_TIMEOUT		1000

#define CMD_REG			0x37
#define  CMD_REG_RESET		0x10

#define CFG_9346		0x50
#define  CFG_9346_LOCK		0x00
#define  CFG_9346_UNLOCK	0xc0

/**
 * search: Find first instance of string in a given region
 * @param p       string to find
 * @param a       start address of region to search
 * @param lengthp length of string to search for
 * @param lengtha length of region to search in
 * @return offset offset from start address a where string was found.
 *                If string not found, return lengtha.
 */
static size_t search(const char *p, const u8 *a, size_t lengthp,
		     size_t lengtha)
{
	size_t i, j;

	if (lengtha < lengthp)
		return lengtha;
	/* Searching */
	for (j = 0; j <= lengtha - lengthp; j++) {
		for (i = 0; i < lengthp && p[i] == a[i + j]; i++)
			;
		if (i >= lengthp)
			return j;
	}
	return lengtha;
}

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
		printk(BIOS_ERR, "Error: Invalid hex digit found: "
				 "%c - 0x%02x\n", (char)c, c);
		ret = 0;
	}
	return ret;
}

#define MACLEN 17

static enum cb_err fetch_mac_string_vpd(u8 *macstrbuf)
{
	struct region_device rdev;
	void *search_address;
	size_t search_length;
	size_t offset;
	char key[] = "ethernet_mac";

	if (fmap_locate_area_as_rdev("RO_VPD", &rdev)) {
		printk(BIOS_ERR, "Error: Couldn't find RO_VPD region.");
		return CB_ERR;
	}
	search_address = rdev_mmap_full(&rdev);
	if (search_address == NULL) {
		printk(BIOS_ERR, "LAN: VPD not found.\n");
		return CB_ERR;
	}

	search_length = region_device_sz(&rdev);
	offset = search(key, search_address, strlen(key),
			search_length);
	if (offset == search_length) {
		printk(BIOS_ERR,
		       "Error: Could not locate '%s' in VPD\n", key);
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "Located '%s' in VPD\n", key);

	offset += sizeof(key);	/* move to next character */

	if (offset + MACLEN > search_length) {
		printk(BIOS_ERR, "Search result too small!\n");
		return CB_ERR;
	}
	memcpy(macstrbuf, search_address + offset, MACLEN);

	return CB_SUCCESS;
}

static enum cb_err fetch_mac_string_cbfs(u8 *macstrbuf)
{
	struct cbfsf fh;
	uint32_t matchraw = CBFS_TYPE_RAW;

	if (!cbfs_boot_locate(&fh, "rt8168-macaddress", &matchraw)) {
		/* check the cbfs for the mac address */
		if (rdev_readat(&fh.data, macstrbuf, 0, MACLEN) != MACLEN) {
			printk(BIOS_ERR, "r8168: Error reading MAC from CBFS\n");
			return CB_ERR;
		}
		return CB_SUCCESS;
	}
	return CB_ERR;
}

static void get_mac_address(u8 *macaddr, const u8 *strbuf)
{
	size_t offset = 0;
	int i;

	if ((strbuf[2] != ':') || (strbuf[5] != ':') ||
	    (strbuf[8] != ':') || (strbuf[11] != ':') ||
	    (strbuf[14] != ':')) {
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

static void program_mac_address(struct device *dev, u16 io_base)
{
	u8 macstrbuf[MACLEN] = { 0 };
	int i = 0;
	/* Default MAC Address of 00:E0:4C:00:C0:B0 */
	u8 mac[6] = { 0x00, 0xe0, 0x4c, 0x00, 0xc0, 0xb0 };

	/* check the VPD for the mac address */
	if (IS_ENABLED(CONFIG_RT8168_GET_MAC_FROM_VPD)) {
		if (fetch_mac_string_vpd(macstrbuf) != CB_SUCCESS)
			printk(BIOS_ERR, "r8168: mac address not found in VPD,"
					 " using default 00:e0:4c:00:c0:b0\n");
	} else {
		if (fetch_mac_string_cbfs(macstrbuf) != CB_SUCCESS)
			printk(BIOS_ERR, "r8168: Error reading MAC from CBFS,"
					 " using default 00:e0:4c:00:c0:b0\n");
	}
	get_mac_address(mac, macstrbuf);

	/* Reset NIC */
	printk(BIOS_DEBUG, "r8168: Resetting NIC...");
	outb(CMD_REG_RESET, io_base + CMD_REG);

	/* Poll for reset, with 1sec timeout */
	while (i < NIC_TIMEOUT && (inb(io_base + CMD_REG) & CMD_REG_RESET)) {
		udelay(1000);
		if (++i >= NIC_TIMEOUT)
			printk(BIOS_ERR, "timeout waiting for nic to reset\n");
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
		printk(BIOS_ERR, "r8168: Error cant find MMIO resource\n");
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
