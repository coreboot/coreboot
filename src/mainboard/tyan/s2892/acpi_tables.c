/*
 * Island Aruma ACPI support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *
 *
 *  Copyright 2005 AMD
 *  2005.9 yhlu modify that to more dynamic for AMD Opteron Based MB
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "northbridge/amd/amdk8/acpi.h"
#include <cpu/amd/powernow.h>

/* APIC */
unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned long apic_addr;
	device_t dev;
	struct resource *res;

	get_bus_conf();

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write NVIDIA CK804 IOAPIC. */
	dev = dev_find_slot(0x0, PCI_DEVFN(0x1,0));
	ASSERT(dev != NULL);

	res = find_resource(dev, PCI_BASE_ADDRESS_1);
	ASSERT(res != NULL);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 4,
					   res->base, 0);
	/* Initialize interrupt mapping if mptable.c didn't. */
#if (!CONFIG_GENERATE_MP_TABLE)
	pci_write_config32(dev, 0x7c, 0x0120d218);
	pci_write_config32(dev, 0x80, 0x12008a00);
	pci_write_config32(dev, 0x84, 0x0000007d);
#endif

	/* Write AMD 8131 two IOAPICs. */
	dev = dev_find_slot(0x40, PCI_DEVFN(0x0,1));
	if (dev) {
		apic_addr = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & ~0xf;
		current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 5,
						   apic_addr, 0x18);
	}

	dev = dev_find_slot(0x40, PCI_DEVFN(0x1, 1));
	if (dev) {
		apic_addr = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & ~0xf;
		current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 6,
						   apic_addr, 0x1C);
	}

	/* IRQ9 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_LOW);

	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */

	/* create all subtables for processors */
	/* acpi_create_madt_lapic_nmis returns current, not size. */
	current = acpi_create_madt_lapic_nmis(current, 5, 1);

	return current;
}
