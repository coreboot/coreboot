/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/tsn_gbe.h>

static void program_mac_address(struct device *dev, void *base)
{
	enum cb_err status;
	uint8_t mac[MAC_ADDR_LEN];

	/* Check first whether there is a valid MAC address available */
	status = mainboard_get_mac_address(dev, mac);
	if (status != CB_SUCCESS) {
		printk(BIOS_INFO, "TSN GbE: No valid MAC address found\n");
		return;
	}

	printk(BIOS_DEBUG, "TSN GbE: Programming MAC Address...\n");

	/* Write the upper 16 bits of the first 6-byte MAC address */
	clrsetbits32(base + TSN_MAC_ADD0_HIGH, 0xffff, ((mac[5] << 8) | mac[4]));
	/* Write the lower 32 bits of the first 6-byte MAC address */
	clrsetbits32(base + TSN_MAC_ADD0_LOW, 0xffffffff,
			(mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0]);
}

static void gbe_tsn_init(struct device *dev)
{
	/* Get the base address of the I/O registers in memory space */
	struct resource *gbe_tsn_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	void *io_mem_base = (void *)(uintptr_t)gbe_tsn_res->base;

	/* Program MAC address */
	program_mac_address(dev, io_mem_base);
}

static struct device_operations gbe_tsn_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = gbe_tsn_init,
};

static const unsigned short gbe_tsn_device_ids[] = { 0x4b32, 0x4ba0, 0x4bb0, 0 };

static const struct pci_driver gbe_tsn_driver __pci_driver = {
	.ops     = &gbe_tsn_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = gbe_tsn_device_ids,
};
