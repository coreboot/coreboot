/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#include <console/console.h>
#include <device/device.h>
#include <device/resource.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "ck804.h"

static void pci_init(struct device *dev)
{
	uint32_t dword;
	device_t pci_domain_dev;
	struct resource *mem, *pref;

	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* System error enable */
	dword |= (1 << 30);	/* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

#if 0
	word = pci_read_config16(dev, 0x48);
	word |= (1 << 0);	/* MRL2MRM */
	word |= (1 << 2);	/* MR2MRM */
	pci_write_config16(dev, 0x48, word);
#endif

#if 1
	dword = pci_read_config32(dev, 0x4c);
	dword |= 0x00440000;	/* TABORT_SER_ENABLE Park Last Enable. */
	pci_write_config32(dev, 0x4c, dword);
#endif

	pci_domain_dev = dev->bus->dev;
	while (pci_domain_dev) {
		if (pci_domain_dev->path.type == DEVICE_PATH_PCI_DOMAIN)
			break;
		pci_domain_dev = pci_domain_dev->bus->dev;
	}

	if (!pci_domain_dev)
		return;		/* Impossible */

	pref = probe_resource(pci_domain_dev, IOINDEX_SUBTRACTIVE(2,0));
	mem = probe_resource(pci_domain_dev, IOINDEX_SUBTRACTIVE(1,0));

	if (!mem)
		return;		/* Impossible */

	if (!pref || pref->base > mem->base) {
		dword = mem->base & (0xffff0000UL);
		printk_debug("PCI DOMAIN mem base = 0x%010Lx\n", mem->base);
	} else {
		dword = pref->base & (0xffff0000UL);
		printk_debug("PCI DOMAIN pref base = 0x%010Lx\n", pref->base);
	}

	printk_debug("[0x50] <-- 0x%08x\n", dword);
	pci_write_config32(dev, 0x50, dword);	/* TOM */
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations pci_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pci_init,
	.scan_bus         = pci_scan_bridge,
	// .enable        = ck804_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_NVIDIA,
	.device = PCI_DEVICE_ID_NVIDIA_CK804_PCI,
};
