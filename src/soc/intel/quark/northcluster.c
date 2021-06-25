/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/iomap.h>
#include <soc/ramstage.h>

static void nc_read_resources(struct device *dev)
{
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* 0 -> 0xa0000 */
	ram_from_to(dev, index++, 0, 0xa0000);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xdffff: RAM
	 * 0xe0000 - 0xfffff: ROM shadow
	 */
	mmio_from_to(dev, index++, 0xa0000, 0xc0000);

	reserved_ram_from_to(dev, index++, 0xc0000, 1 * MiB);

	/* 0x100000 -> cbmem_top - cacheable and usable */
	ram_from_to(dev, index++, 1 * MiB, (uintptr_t)cbmem_top());

	/* cbmem_top -> 0xc0000000 - reserved */
	reserved_ram_from_to(dev, index++, (uintptr_t)cbmem_top(), 0xc0000000);

	/* 0xc0000000 -> 4GiB is mmio. */
	mmio_from_to(dev, index++, 0xc0000000, 4ull * GiB);
}

static struct device_operations nc_ops = {
	.read_resources   = nc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
};

static const struct pci_driver systemagent_driver __pci_driver = {
	.ops	= &nc_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= QUARK_MC_DEVICE_ID
};
