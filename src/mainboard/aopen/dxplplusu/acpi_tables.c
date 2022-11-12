/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/pci_def.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *bdev, *dev = NULL;
	struct resource *res = NULL;

	current = acpi_create_madt_lapics_with_nmis(current);

	/* Southbridge IOAPIC */
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, 0xfec00000);

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

	/* Map ISA IRQ 0 to IRQ 2 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 1, 0, 2, 0);

	/* IRQ9 differs from ISA standard - ours is active high, level-triggered */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 0, 9, 9, 0xD);

	return current;
}
