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
#include <cpu/amd/model_fxx_powernow.h>

extern const unsigned char AmlCode[];

unsigned long acpi_fill_mcfg(unsigned long current)
{
	return current;
}

/* APIC */
unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned long apic_addr;
	device_t dev;
	struct resource *res;

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
	pci_write_config32(dev, 0x84, 0x00080d7d);
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

	/* Write second NVIDIA CK804 IOAPIC. */
	dev = dev_find_slot(0x80, PCI_DEVFN(0x1, 0));
	ASSERT(dev != NULL);

	res = find_resource(dev, PCI_BASE_ADDRESS_1);
	ASSERT(res != NULL);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 7,
					   res->base, 0x20);
	/* Initialize interrupt mapping if mptable.c didn't. */
#if (!CONFIG_GENERATE_MP_TABLE)
	pci_write_config32(dev, 0x7c, 0x0000d218); // Why does the factory BIOS have 0?
	pci_write_config32(dev, 0x80, 0x00000000);
	pci_write_config32(dev, 0x84, 0x00000d00); // Same here.
#endif

	/* IRQ9 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_LOW);

	/* IRQ0 -> APIC IRQ2. */
	/* Doesn't work on this board. */

	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */

	/* create all subtables for processors */
	/* acpi_create_madt_lapic_nmis returns current, not size. */
	current = acpi_create_madt_lapic_nmis(current, 5, 1);

	return current;
}

unsigned long acpi_fill_ssdt_generator(unsigned long current, const char *oem_table_id) {
	k8acpi_write_vars();
	amd_model_fxx_generate_powernow(0, 0, 0);
	return (unsigned long) (acpigen_get_current());
}

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_srat_t *srat;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_slit_t *slit;
	acpi_header_t *ssdt;
	acpi_header_t *dsdt;

	/* Align ACPI tables to 16 byte. */
	start = ALIGN(start, 16);
	current = start;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);

	current = ALIGN(current, 16);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* Clear all table memory. */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, NULL);
	acpi_write_rsdt(rsdt);

	current = ALIGN(current, 64);
	facs = (acpi_facs_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * FACS %p\n", facs);
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	/* DSDT */
	current = ALIGN(current, 16);
	dsdt = (acpi_header_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * DSDT %p\n", dsdt);
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	memcpy(dsdt, &AmlCode, dsdt->length);
	printk(BIOS_DEBUG, "ACPI:    * DSDT @ %p Length %x\n",dsdt,dsdt->length);

	current = ALIGN(current, 16);
	fadt = (acpi_fadt_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * FACP (FADT) @ %p\n", fadt);
	current += sizeof(acpi_fadt_t);

	/* Add FADT now that we have facs and dsdt. */
	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

	current = ALIGN(current, 16);
	hpet = (acpi_hpet_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * HPET @ %p\n", hpet);
	current += sizeof(acpi_hpet_t);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	current = ALIGN(current, 16);
	madt = (acpi_madt_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * APIC/MADT @ %p\n", madt);
	acpi_create_madt(madt);
	current += madt->header.length;
	acpi_add_table(rsdp, madt);

	current = ALIGN(current, 16);
	srat = (acpi_srat_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * SRAT @ %p\n", srat);
	acpi_create_srat(srat);
	current += srat->header.length;
	acpi_add_table(rsdp, srat);

	/* SLIT */
	current = ALIGN(current, 16);
	slit = (acpi_slit_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * SLIT @ %p\n", slit);
	acpi_create_slit(slit);
	current+=slit->header.length;
	acpi_add_table(rsdp,slit);

	/* SSDT */
	current = ALIGN(current, 16);
	ssdt = (acpi_header_t *)current;
	printk(BIOS_DEBUG, "ACPI:    * SSDT @ %p\n", ssdt);
	acpi_create_ssdt_generator(ssdt, ACPI_TABLE_CREATOR);
	current += ssdt->length;
	acpi_add_table(rsdp, ssdt);

	printk(BIOS_INFO, "ACPI: done %p.\n", (void *)current);
	return current;
}
