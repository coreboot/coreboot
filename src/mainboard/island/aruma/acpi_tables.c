/*
 * Island Aruma ACPI support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>

extern unsigned char AmlCode[];
extern unsigned char oem_config[];

#define IO_APIC_ADDR	0xfec00000UL

unsigned long acpi_dump_apics(unsigned long current)
{
	unsigned int gsi_base=0x18;
	/* create all subtables for 4p */
	/* CPUs are called 1234 in regular bios */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 16);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 1, 17);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 2, 18);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 3, 19);
	
	/* Write 8111 IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, 1,
			IO_APIC_ADDR, 0);

        /* Write all 8131 IOAPICs */
        /* (8131: bus, dev, fn) , id, version */
        ACPI_WRITE_MADT_IOAPIC(0x01,1,1, 2);
        ACPI_WRITE_MADT_IOAPIC(0x01,2,1, 3);
        ACPI_WRITE_MADT_IOAPIC(0x05,1,1, 4);
        ACPI_WRITE_MADT_IOAPIC(0x05,2,1, 5);
        ACPI_WRITE_MADT_IOAPIC(0x05,3,1, 6);
        ACPI_WRITE_MADT_IOAPIC(0x05,4,1, 7);
        ACPI_WRITE_MADT_IOAPIC(0x0c,1,1, 8);
        ACPI_WRITE_MADT_IOAPIC(0x0c,2,1, 9);
        ACPI_WRITE_MADT_IOAPIC(0x0c,3,1, 10);
        ACPI_WRITE_MADT_IOAPIC(0x0c,4,1, 11);
        ACPI_WRITE_MADT_IOAPIC(0x11,1,1, 12);
        ACPI_WRITE_MADT_IOAPIC(0x11,2,1, 13);
        ACPI_WRITE_MADT_IOAPIC(0x11,3,1, 14);
        ACPI_WRITE_MADT_IOAPIC(0x11,4,1, 15);
	
	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 1, 0, 2, 0 );

	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 1, 0, 2, 0 );

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
	acpi_header_t *oemb;
	
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

	printk_debug("ACPI:    * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *)current;
	current += ((acpi_header_t *)AmlCode)->length;
	memcpy((void *)dsdt,(void *)AmlCode, \
			((acpi_header_t *)AmlCode)->length);

	/* recalculate checksum */
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum(dsdt,dsdt->length);
	printk_debug("ACPI:    * DSDT @ %08x Length %x\n",dsdt,dsdt->length);
	printk_debug("ACPI:    * FADT\n");

	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt,facs,dsdt);
	acpi_add_table(rsdt,fadt);

	printk_info("ACPI: done.\n");
	return current;
}

