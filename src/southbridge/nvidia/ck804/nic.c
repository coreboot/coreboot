/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "ck804.h"

static void nic_init(struct device *dev)
{
	u32 dword, old, mac_h = 0, mac_l = 0;
	int eeprom_valid = 0;
	struct southbridge_nvidia_ck804_config *conf;
	static u32 nic_index = 0;
	unsigned long base;
	struct resource *res;

	res = find_resource(dev, 0x10);
	base = (unsigned long)res->base;

#define NvRegPhyInterface  0xC0
#define PHY_RGMII          0x10000000

	write32(base + NvRegPhyInterface, PHY_RGMII);

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

	/* If that is invalid we will read that from romstrap. */
	if (!eeprom_valid) {
		unsigned long mac_pos;
		mac_pos = 0xffffffd0; /* See romstrap.inc and romstrap.lds. */
		mac_l = read32(mac_pos) + nic_index;
		mac_h = read32(mac_pos + 4);
	}
#if 1
	/* Set that into NIC MMIO. */
#define NvRegMacAddrA  0xA8
#define NvRegMacAddrB  0xAC
	write32(base + NvRegMacAddrA, mac_l);
	write32(base + NvRegMacAddrB, mac_h);
#else
	/* Set that into NIC. */
	pci_write_config32(dev, 0xa8, mac_l);
	pci_write_config32(dev, 0xac, mac_h);
#endif

	nic_index++;

#if CONFIG_PCI_ROM_RUN
	pci_dev_init(dev);	/* It will init Option ROM. */
#endif
}

static struct device_operations nic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = nic_init,
	.scan_bus         = 0,
	// .enable        = ck804_enable,
	.ops_pci          = &ck804_pci_ops,
};

static const struct pci_driver nic_driver __pci_driver = {
	.ops    = &nic_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_NIC,
};

static const struct pci_driver nic_bridge_driver __pci_driver = {
	.ops    = &nic_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_NIC_BRIDGE,
};
