/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Temperature at which OS will shutdown */
	gnvs->tcrt = 100;
	/* Temperature at which OS will throttle CPU */
	gnvs->tpsv = 90;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	/* LAPIC_NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
				current, 0,
				MP_IRQ_POLARITY_HIGH |
				MP_IRQ_TRIGGER_EDGE, 0x01);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
				current, 1, MP_IRQ_POLARITY_HIGH |
				MP_IRQ_TRIGGER_EDGE, 0x01);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 0, 2, MP_IRQ_POLARITY_HIGH | MP_IRQ_TRIGGER_EDGE);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 9, 9, MP_IRQ_POLARITY_HIGH | MP_IRQ_TRIGGER_LEVEL);

	return current;
}
