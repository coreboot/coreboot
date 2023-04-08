/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/pci_def.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *bdev, *dev = NULL;
	struct resource *res = NULL;

	bdev = pcidev_on_root(2, 0);
	/* P64H2 Bus B IOAPIC */
	if (bdev)
		dev = pcidev_path_behind(bdev->link_list, PCI_DEVFN(28, 0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_0);
		current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, res->base);
	}

	/* P64H2 Bus A IOAPIC */
	if (bdev)
		dev = pcidev_path_behind(bdev->link_list, PCI_DEVFN(30, 0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_0);
		current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, res->base);
	}

	return current;
}
