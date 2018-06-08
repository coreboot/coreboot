/*
 * ACPI support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *
 *
 *  Copyright 2005 AMD
 *  2005.9 yhlu modify that to more dynamic for AMD Opteron Based MB
 */

#include <arch/acpi.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <assert.h>

/* APIC */
unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *dev;
	struct resource *res;

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write NVIDIA CK804 IOAPIC. */
	dev = dev_find_slot(0x0, PCI_DEVFN(0x1,0));
	ASSERT(dev != NULL);

	res = find_resource(dev, PCI_BASE_ADDRESS_1);
	ASSERT(res != NULL);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
		CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS, res->base, 0);

	/* Initialize interrupt mapping if mptable.c didn't. */
#if (!CONFIG_GENERATE_MP_TABLE)
#error untested config
	pci_write_config32(dev, 0x7c, 0x0120d218);
	pci_write_config32(dev, 0x80, 0x12008a00);
	pci_write_config32(dev, 0x84, 0x0000007d);
#endif

	/* IRQ9 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	/* create all subtables for processors */
	/* acpi_create_madt_lapic_nmis returns current, not size. */
	current = acpi_create_madt_lapic_nmis(current,
			MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);

	return current;
}
