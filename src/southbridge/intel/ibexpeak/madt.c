/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics_with_nmis(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC_ADDR);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2,
						MP_IRQ_POLARITY_DEFAULT |
						MP_IRQ_TRIGGER_DEFAULT);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9,
						MP_IRQ_POLARITY_HIGH |
						MP_IRQ_TRIGGER_LEVEL);

	return current;
}
