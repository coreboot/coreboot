/*
 * This file is part of the coreboot project.
 *
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <io.h>
#include "ck804.h"

static void nic_init(struct device *dev)
{
	u32 dword, old, mac_h, mac_l;
	int eeprom_valid = 0;
	struct southbridge_nvidia_ck804_config *conf;
	static u32 nic_index = 0;
	u8 *base;
	struct resource *res;

	res = find_resource(dev, 0x10);
	base = res->base;

#define NvRegPhyInterface  0xC0
#define PHY_RGMII          0x10000000

	writel(PHY_RGMII, base + NvRegPhyInterface);

	old = dword = pci_read_config32(dev, 0x30);
	dword &= ~(0xf);
	dword |= 0xf;
	if (old != dword)
		pci_write_config32(dev, 0x30, dword);

	conf = dev->chip_info;

	if (conf->mac_eeprom_smbus != 0) {
		/* Read MAC address from EEPROM at first. */
		struct device *dev_eeprom;
		dev_eeprom = dev_find_slot_on_smbus(conf->mac_eeprom_smbus,
					   conf->mac_eeprom_addr);

		if (dev_eeprom) {
			/* If that is valid we will use that. */
			unsigned char dat[6];
			int i, status;
			for (i = 0; i < 6; i++) {
				status = smbus_read_byte(dev_eeprom, i);
				if (status < 0)
					break;
				dat[i] = status & 0xff;
			}
			if (status >= 0) {
				mac_l = 0;
				for (i = 3; i >= 0; i--) {
					mac_l <<= 8;
					mac_l += dat[i];
				}
				if (mac_l != 0xffffffff) {
					mac_l += nic_index;
					mac_h = 0;
					for (i = 5; i >= 4; i--) {
						mac_h <<= 8;
						mac_h += dat[i];
					}
					eeprom_valid = 1;
				}
			}
		}
	}

#warning This should be fixed.  Can we use a LAR file for the MAC?

	/* If that is invalid we will read that from romstrap. */
	if (!eeprom_valid) {
		unsigned long mac_pos;
		mac_pos = 0xffffffd0; /* See romstrap.inc and romstrap.lds. */
		mac_l = readl(mac_pos) + nic_index;
		mac_h = readl(mac_pos + 4);
	}
#if 1
	/* Set that into NIC MMIO. */
#define NvRegMacAddrA  0xA8
#define NvRegMacAddrB  0xAC
	writel(mac_l, base + NvRegMacAddrA);
	writel(mac_h, base + NvRegMacAddrB);
#else
	/* Set that into NIC. */
	pci_write_config32(dev, 0xa8, mac_l);
	pci_write_config32(dev, 0xac, mac_h);
#endif

	nic_index++;

#if CONFIG_PCI_ROM_RUN == 1
	pci_dev_init(dev);	/* It will init Option ROM. */
#endif
}

struct device_operations ck804_nic_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			 .device = PCI_DEVICE_ID_NVIDIA_CK804_NIC}}},
	.phase3_chip_setup_dev	 = ck804_enable,
	.phase3_scan		 = NULL,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = nic_init,
	.ops_pci		 = &ck804_ops_pci,
};

struct device_operations ck804_nic_bridge_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			 .device = PCI_DEVICE_ID_NVIDIA_CK804_NIC_BRIDGE}}},
	.phase3_chip_setup_dev	 = ck804_enable,
	.phase3_scan		 = NULL,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = nic_init,
	.ops_pci		 = &ck804_ops_pci,
};

