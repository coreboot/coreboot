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
#include <arch/acpi_device.h>
#include <arch/acpigen.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <delay.h>
#include <fmap.h>
#include "chip.h"

#define NIC_TIMEOUT		1000

#define CMD_REG			0x37
#define  CMD_REG_RESET		0x10
#define CMD_LED0_LED1		0x18

#define CFG_9346		0x50
#define  CFG_9346_LOCK		0x00
#define  CFG_9346_UNLOCK	0xc0

#define DEVICE_INDEX_BYTE	12
#define MAX_DEVICE_SUPPORT	10
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
		if (i >= lengthp && a[j - 1] == lengthp)
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

static enum cb_err fetch_mac_string_vpd(u8 *macstrbuf, const u8 device_index)
{
	struct region_device rdev;
	void *search_address;
	size_t search_length;
	size_t offset;
	char key[] = "ethernet_mac "; /* Leave a space at tail to stuff an index */

	/*
	 * The device_index 0 is treated as an special case matching to
	 * "ethernet_mac" with single NIC on DUT. When there are mulitple
	 * NICs on DUT, they are mapping to "ethernet_macN", where
	 * N is [0-9].
	 */
	if (device_index == 0)
		key[DEVICE_INDEX_BYTE] = '\0';
	else
		/* Translate index number from integer to ascii */
		key[DEVICE_INDEX_BYTE] = (device_index - 1) + '0';

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

	offset += strlen(key) + 1;	/* move to next character */

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
	struct drivers_net_config *config = dev->chip_info;
	bool mac_found = false;

	/* check the VPD for the mac address */
	if (IS_ENABLED(CONFIG_RT8168_GET_MAC_FROM_VPD)) {
		/* Current implementation is up to 10 NIC cards */
		if (config && config->device_index <= MAX_DEVICE_SUPPORT) {
			/* check "ethernet_mac" first when the device index is 1 */
			if (config->device_index == 1 &&
				fetch_mac_string_vpd(macstrbuf, 0) == CB_SUCCESS)
				mac_found = true;
			if (!mac_found && fetch_mac_string_vpd(macstrbuf,
				config->device_index) != CB_SUCCESS)
				printk(BIOS_ERR, "r8168: mac address not found in VPD,"
								 " using default 00:e0:4c:00:c0:b0\n");
		} else {
			printk(BIOS_ERR, "r8168: the maximum device_index should be"
						" less then %d\n. Using default 00:e0:4c:00:c0:b0\n",
						MAX_DEVICE_SUPPORT);
		}
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

static void r8168_set_customized_led(struct device *dev, u16 io_base)
{
	struct drivers_net_config *config = dev->chip_info;

	if (!config)
		return;

	/* Read the customized LED setting from devicetree */
	printk(BIOS_DEBUG, "r8168: Customized LED 0x%x\n", config->customized_leds);

	/*
	 * Refer to RTL8111H datasheet 7.2 Customizable LED Configuration
	 * Starting from offset 0x18
	 * Bit[15:12]	LED Feature Control(FC)
	 * Bit[11:08]	LED Select for PINLED2
	 * Bit[07:04]	LED Select for PINLED1
	 * Bit[03:00]	LED Select for PINLED0
	 *
	 * Speed	Link10M		Link100M	Link1000M	ACT/Full
	 * LED0		Bit0		Bit1		Bit2		Bit3
	 * LED1		Bit4		Bit5		Bit6		Bit7
	 * LED2		Bit8		Bit9		Bit10		Bit11
	 * FC		Bit12		Bit13		Bit14		Bit15
	 */

	/* Set customized LED registers */
	outw(config->customized_leds, io_base + CMD_LED0_LED1);
	printk(BIOS_DEBUG, "r8168: read back LED setting as 0x%x\n",
		inw(io_base + CMD_LED0_LED1));
}

static void r8168_init(struct device *dev)
{
	/* Get the resource of the NIC mmio */
	struct resource *nic_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	u16 io_base = (u16)nic_res->base;

	/* Check if the base is invalid */
	if (!io_base) {
		printk(BIOS_ERR, "r8168: Error cant find IO resource\n");
		return;
	}
	/* Enable but do not set bus master */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Program MAC address based on CBFS "macaddress" containing
	 * a string AA:BB:CC:DD:EE:FF */
	program_mac_address(dev, io_base);

	/* Program customized LED mode */
	if (IS_ENABLED(CONFIG_RT8168_SET_LED_MODE))
		r8168_set_customized_led(dev, io_base);
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
#define R8168_ACPI_HID "R8168"
static void r8168_net_fill_ssdt(struct device *dev)
{
	struct drivers_net_config *config = dev->chip_info;
	const char *path = acpi_device_path(dev->bus->dev);
	u32 address;

	if (!path || !config)
		return;

	/* Device */
	acpigen_write_scope(path);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", R8168_ACPI_HID);
	acpigen_write_name_integer("_UID", 0);
	if (dev->chip_ops)
		acpigen_write_name_string("_DDN", dev->chip_ops->name);

	/* Address */
	address = PCI_SLOT(dev->path.pci.devfn) & 0xffff;
	address <<= 16;
	address |= PCI_FUNC(dev->path.pci.devfn) & 0xffff;
	acpigen_write_name_dword("_ADR", address);

	/* Wake capabilities */
	if (config->wake)
		acpigen_write_PRW(config->wake, 3);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s.%s: %s %s\n", path, acpi_device_name(dev),
		dev->chip_ops ? dev->chip_ops->name : "", dev_path(dev));
}

static const char *r8168_net_acpi_name(const struct device *dev)
{
	return "RLTK";
}
#endif

static struct device_operations r8168_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = r8168_init,
	.scan_bus         = 0,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name                = &r8168_net_acpi_name,
	.acpi_fill_ssdt_generator = &r8168_net_fill_ssdt,
#endif
};

static const struct pci_driver r8168_driver __pci_driver = {
	.ops    = &r8168_ops,
	.vendor = 0x10ec,
	.device = 0x8168,
};

struct chip_operations drivers_net_ops = {
	CHIP_NAME("Realtek r8168")
};
