/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Ported to Intel XE7501DEVKIT by Agami Aruma
 * Ported to AOpen DXPL Plus-U by Kyösti Mälkki
 */

#include <acpi/acpi.h>
#include <device/pci_def.h>

#define IOAPIC_ICH4		2
#define IOAPIC_P64H2_BUS_B	3	/* IOAPIC 3 at 02:1c.0 */
#define IOAPIC_P64H2_BUS_A	4	/* IOAPIC 4 at 02:1e.0 */

#define INTEL_IOAPIC_NUM_INTERRUPTS	24	/* Both ICH-4 and P64-H2 */

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int irq_start = 0;
	struct device *bdev, *dev = NULL;
	struct resource *res = NULL;

	/* SJM: Hard-code CPU LAPIC entries for now */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 0);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 1, 6);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 2, 1);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 3, 7);

	/* Southbridge IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, IOAPIC_ICH4,
					   0xfec00000, irq_start);
	irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;

	bdev = pcidev_on_root(2, 0);
	/* P64H2 Bus B IOAPIC */
	if (bdev)
		dev = pcidev_path_behind(bdev->link_list, PCI_DEVFN(28, 0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_0);
		current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
						   IOAPIC_P64H2_BUS_B, res->base, irq_start);
		irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;
	}

	/* P64H2 Bus A IOAPIC */
	if (bdev)
		dev = pcidev_path_behind(bdev->link_list, PCI_DEVFN(30, 0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_0);
		current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
						   IOAPIC_P64H2_BUS_A, res->base, irq_start);
		irq_start += INTEL_IOAPIC_NUM_INTERRUPTS;
	}

	/* Map ISA IRQ 0 to IRQ 2 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 1, 0, 2, 0);

	/* IRQ9 differs from ISA standard - ours is active high, level-triggered */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 0, 9, 9, 0xD);

	return current;
}
