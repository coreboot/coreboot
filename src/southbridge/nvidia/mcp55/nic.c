/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "mcp55.h"

static int phy_read(u8 *base, unsigned phy_addr, unsigned phy_reg)
{
	u32 dword;
	unsigned loop = 0x100;

	write32(base + 0x190, 0x8000); /* Clear MDIO lock bit. */
	mdelay(1);
	dword = read32(base + 0x190);
	if (dword & (1 << 15))
		return -1;

	write32(base + 0x180, 1);
	write32(base + 0x190, (phy_addr << 5) | (phy_reg));
	do {
		dword = read32(base + 0x190);
		if (--loop == 0)
			return -4;
	} while ((dword & (1 << 15)));

	dword = read32(base + 0x180);
	if (dword & 1)
		return -3;

	dword = read32(base + 0x194);

	return dword;
}

static void phy_detect(u8 *base)
{
	u32 dword;
	int i, val;
	unsigned id;

	dword = read32(base + 0x188);
	dword &= ~(1 << 20);
	write32(base + 0x188, dword);

	phy_read(base, 0, 1);

	for (i = 1; i <= 32; i++) {
		int phyaddr = i & 0x1f;
		val = phy_read(base, phyaddr, 1);
		if (val < 0)
			continue;
		if ((val & 0xffff) == 0xffff)
			continue;
		if ((val & 0xffff) == 0)
			continue;
		if (!(val & 1))
			break; /* Ethernet PHY */

		val = phy_read(base, phyaddr, 3);
		if (val < 0 || val == 0xffff)
			continue;
		id = val & 0xfc00;
		val = phy_read(base, phyaddr, 2);
		if (val < 0 || val == 0xffff)
			continue;
		id |= ((val & 0xffff) << 16);
		printk(BIOS_DEBUG, "MCP55 MAC PHY ID 0x%08x PHY ADDR %d\n",
		       id, i);
//		if ((id == 0xe0180000) || (id == 0x0032cc00))
			break;
	}

	if (i > 32)
		printk(BIOS_DEBUG, "MCP55 MAC PHY not found\n");
}

static void nic_init(struct device *dev)
{
	u8 *base;
	u32 mac_h = 0, mac_l = 0;
	int eeprom_valid = 0;
	struct southbridge_nvidia_mcp55_config *conf;
	static u32 nic_index = 0;
	struct resource *res;

	res = find_resource(dev, 0x10);

	if (!res)
		return;

	base = res2mmio(res, 0, 0);

	phy_detect(base);

#define NvRegPhyInterface	0xC0
#define PHY_RGMII		0x10000000

	write32(base + NvRegPhyInterface, PHY_RGMII);

	conf = dev->chip_info;

	if (conf->mac_eeprom_smbus != 0) {
//	read MAC address from EEPROM at first
		struct device *dev_eeprom;
		dev_eeprom = dev_find_slot_on_smbus(conf->mac_eeprom_smbus, conf->mac_eeprom_addr);

		if (dev_eeprom) {
		//	if that is valid we will use that
			unsigned char dat[6];
			int status;
			int i;
			for (i=0;i<6;i++) {
				status = smbus_read_byte(dev_eeprom, i);
				if (status < 0) break;
				dat[i] = status & 0xff;
			}
			if (status >= 0) {
				mac_l = 0;
				for (i=3;i>=0;i--) {
					mac_l <<= 8;
					mac_l += dat[i];
				}
				if (mac_l != 0xffffffff) {
					mac_l += nic_index;
					mac_h = 0;
					for (i=5;i>=4;i--) {
						mac_h <<= 8;
						mac_h += dat[i];
					}
					eeprom_valid = 1;
				}
			}
		}
	}
//	if that is invalid we will read that from romstrap
	if (!eeprom_valid) {
		u32 *mac_pos;
		mac_pos = (u32 *)0xffffffd0; // refer to romstrap.inc and romstrap.ld
		mac_l = read32(mac_pos) + nic_index; // overflow?
		mac_h = read32(mac_pos + 1);

	}
#if 1
//	set that into NIC MMIO
#define NvRegMacAddrA	0xA8
#define NvRegMacAddrB	0xAC
	write32(base + NvRegMacAddrA, mac_l);
	write32(base + NvRegMacAddrB, mac_h);
#else
//	set that into NIC
	pci_write_config32(dev, 0xa8, mac_l);
	pci_write_config32(dev, 0xac, mac_h);
#endif

	nic_index++;
}

static struct device_operations nic_ops  = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= nic_init,
	.scan_bus	= 0,
//	.enable		= mcp55_enable,
	.ops_pci	= &mcp55_pci_ops,
};
static const struct pci_driver nic_driver __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.device	= PCI_DEVICE_ID_NVIDIA_MCP55_NIC,
};
static const struct pci_driver nic_bridge_driver __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.device	= PCI_DEVICE_ID_NVIDIA_MCP55_NIC_BRIDGE,
};
