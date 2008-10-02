/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
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
	uint32_t dword, old, mac_h, mac_l;
	int eeprom_valid = 0;
	struct southbridge_nvidia_ck804_config *conf;
	static uint32_t nic_index = 0;
	uint8_t *base;
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

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations nic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = nic_init,
	.scan_bus         = 0,
	// .enable        = ck804_enable,
	.ops_pci          = &lops_pci,
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
