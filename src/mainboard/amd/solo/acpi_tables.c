/*
 * LinuxBIOS ACPI support for AMD Solo
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>


unsigned long acpi_dump_apics(unsigned long current)
{
#define IO_APIC_ADDR	0xfec00000UL
	/* create all subtables for 1p */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 0);
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, 2,
			IO_APIC_ADDR);
	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 0, 0, 2, 1 /* active high */);
	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 0, 9, 9, 0xf /* active low, level triggered */);
	current += acpi_create_madt_lapic_nmi( (acpi_madt_lapic_nmi_t *)
			current, 0, 5, 1);
	return current;
}

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *dsdt;
	
	/* Align ACPI tables to 16byte */
	start   = ( start + 0x0f ) & -0x10;
	current = start;
	
	printk_info("ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* clear all table memory */
	memset((void *)start, 0, current - start);
	
	acpi_write_rsdp(rsdp, rsdt);
	acpi_write_rsdt(rsdt);
	
	/*
	 * We explicitly add these tables later on:
	 */
	printk_debug("ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdt,hpet);

	/* If we want to use HPET Timers Linux wants an MADT */
	printk_debug("ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current+=madt->header.length;
	acpi_add_table(rsdt,madt);

#ifdef HAVE_ACPI_FADT
	printk_debug("ACPI:     * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);


	dsdt = (acpi_header_t *)current;
	current += ((acpi_header_t *)AmlCode)->length;
	memcpy((void *)dsdt,(void *)AmlCode, ((acpi_header_t *)AmlCode)->length);
	dsdt->checksum = 0; // don't trust intel iasl compiler to get this right
	dsdt->checksum = acpi_checksum(dsdt,dsdt->length);
	printk_debug("ACPI:     * DSDT @ %08x Length %x\n",dsdt,dsdt->length);
	printk_debug("ACPI:     * FADT\n");

	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt,facs,dsdt);
	acpi_add_table(rsdt,fadt);
#endif	

	printk_info("ACPI: done.\n");
	return current;
}

