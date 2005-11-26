/*
 * LinuxBIOS ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2004 SUSE LINUX AG
 *  (C) 2005 Stefan Reinauer
 *
 * ACPI FADT, FACS, and DSDT table support added by 
 * Nick Barker <nick.barker9@btinternet.com>, and those portions
 *  (C) Copyright 2004 Nick Barker
 *
 * Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
 * 2005.9 yhlu add SRAT relate
 */

/* 
 * Each system port implementing ACPI has to provide two functions:
 * 
 *   write_acpi_tables()
 *   acpi_dump_apics()
 *   
 * See AMD Solo, Island Aruma or Via Epia-M port for more details.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>

u8 acpi_checksum(u8 *table, u32 length)
{
	u8 ret=0;
	while (length--) {
		ret += *table;
		table++;
	}
	return -ret;
}

/*
 * add an acpi table to rsdt structure, and recalculate checksum
 */

void acpi_add_table(acpi_rsdt_t *rsdt, void *table)
{
	int i;
	
	int entries_num = sizeof(rsdt->entry)/sizeof(rsdt->entry[0]);
	
	for (i=0; i<entries_num; i++) {
		if(rsdt->entry[i]==0) {
			rsdt->entry[i]=(u32)table;
			/* fix length to stop kernel winging about invalid entries */
			rsdt->header.length = sizeof(acpi_header_t) + (sizeof(u32) * (i+1));
			/* fix checksum */
			/* hope this won't get optimized away */
			rsdt->header.checksum=0;
			rsdt->header.checksum=acpi_checksum((u8 *)rsdt,
					rsdt->header.length);
			
			printk_debug("ACPI: added table %d/%d Length now %d\n",i+1, entries_num, rsdt->header.length);
			return;
		}
	}

	printk_warning("ACPI: could not add ACPI table to RSDT. failed.\n");
}


int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic)
{
	lapic->type=0;
	lapic->length=sizeof(acpi_madt_lapic_t);
	lapic->flags=1;
	
	lapic->processor_id=cpu;
	lapic->apic_id=apic;
	
	return(lapic->length);
}

int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,u32 gsi_base) 
{
	ioapic->type=1;
	ioapic->length=sizeof(acpi_madt_ioapic_t);
	ioapic->reserved=0x00;
	ioapic->gsi_base=gsi_base;
	
	ioapic->ioapic_id=id;
	ioapic->ioapic_addr=addr;
	
	return(ioapic->length);
}

int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
		u8 bus, u8 source, u32 gsirq, u16 flags)
{
	irqoverride->type=2;
	irqoverride->length=sizeof(acpi_madt_irqoverride_t);
	irqoverride->bus=bus;
	irqoverride->source=source;
	irqoverride->gsirq=gsirq;
	irqoverride->flags=flags;
	
	return(irqoverride->length);
}

int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
		u16 flags, u8 lint)
{
	lapic_nmi->type=4;
	lapic_nmi->length=sizeof(acpi_madt_lapic_nmi_t);
	
	lapic_nmi->flags=flags;
	lapic_nmi->processor_id=cpu;
	lapic_nmi->lint=lint;
	
	return(lapic_nmi->length);
}

void acpi_create_madt(acpi_madt_t *madt)
{
#define LOCAL_APIC_ADDR	0xfee00000ULL
	
	acpi_header_t *header=&(madt->header);
	unsigned long current=(unsigned long)madt+sizeof(acpi_madt_t);
	
	memset((void *)madt, 0, sizeof(acpi_madt_t));
	
	/* fill out header fields */
	memcpy(header->signature, MADT_NAME, 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, MADT_TABLE, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	
	header->length = sizeof(acpi_madt_t);
	header->revision = 1;

	madt->lapic_addr= LOCAL_APIC_ADDR;
	madt->flags	= 0x1; /* PCAT_COMPAT */

	current = acpi_fill_madt(current);
	
	/* recalculate length */
	header->length= current - (unsigned long)madt;
	
	header->checksum	= acpi_checksum((void *)madt, header->length);
}

int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic)
{
        lapic->type=0;
        lapic->length=sizeof(acpi_srat_lapic_t);
        lapic->flags=1;

        lapic->proximity_domain_7_0 = node;
        lapic->apic_id=apic;

        return(lapic->length);
}

int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek,u32 sizek, u32 flags)
{
        mem->type=1;
        mem->length=sizeof(acpi_srat_mem_t);

        mem->base_address_low = (basek<<10);
	mem->base_address_high = (basek>>(32-10));

        mem->length_low = (sizek<<10);
        mem->length_high = (sizek>>(32-10));

        mem->proximity_domain = node;

	mem->flags = flags; 

        return(mem->length);
}

void acpi_create_srat(acpi_srat_t *srat)
{

        acpi_header_t *header=&(srat->header);
        unsigned long current=(unsigned long)srat+sizeof(acpi_srat_t);

        memset((void *)srat, 0, sizeof(acpi_srat_t));

        /* fill out header fields */
        memcpy(header->signature, SRAT_NAME, 4);
        memcpy(header->oem_id, OEM_ID, 6);
        memcpy(header->oem_table_id, SRAT_TABLE, 8);
        memcpy(header->asl_compiler_id, ASLC, 4);

        header->length = sizeof(acpi_srat_t);
        header->revision = 1;

        srat->resv     = 0x1; /* BACK COMP */

        current = acpi_fill_srat(current);

        /* recalculate length */
        header->length= current - (unsigned long)srat;

        header->checksum        = acpi_checksum((void *)srat, header->length);
}

void acpi_create_hpet(acpi_hpet_t *hpet)
{
#define HPET_ADDR  0xfed00000ULL
	acpi_header_t *header=&(hpet->header);
	acpi_addr_t *addr=&(hpet->addr);
	
	memset((void *)hpet, 0, sizeof(acpi_hpet_t));
	
	/* fill out header fields */
	memcpy(header->signature, HPET_NAME, 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, HPET_TABLE, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	
	header->length = sizeof(acpi_hpet_t);
	header->revision = 1;

	/* fill out HPET address */
	addr->space_id		= 0; /* Memory */
	addr->bit_width		= 64;
	addr->bit_offset	= 0;
	addr->addrl		= HPET_ADDR & 0xffffffff;
	addr->addrh		= HPET_ADDR >> 32;

	hpet->id	= 0x102282a0; /* AMD ? */
	hpet->number	= 0;
	hpet->min_tick  = 4096;
	
	header->checksum	= acpi_checksum((void *)hpet, sizeof(acpi_hpet_t));
}

void acpi_create_facs(acpi_facs_t *facs)
{
	memset( (void *)facs,0, sizeof(acpi_facs_t));

	memcpy(facs->signature,"FACS",4);
	facs->length = sizeof(acpi_facs_t);
	facs->hardware_signature = 0;
	facs->firmware_waking_vector = 0;
	facs->global_lock = 0;
	facs->flags = 0;
	facs->x_firmware_waking_vector_l = 0;
	facs->x_firmware_waking_vector_h = 0;
	facs->version = 1;
}

void acpi_write_rsdt(acpi_rsdt_t *rsdt)
{ 
	acpi_header_t *header=&(rsdt->header);
	
	/* fill out header fields */
	memcpy(header->signature, RSDT_NAME, 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, RSDT_TABLE, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	
	header->length = sizeof(acpi_rsdt_t);
	header->revision = 1;
	
	/* fill out entries */

	// entries are filled in later, we come with an empty set.
	
	/* fix checksum */
	
	header->checksum	= acpi_checksum((void *)rsdt, sizeof(acpi_rsdt_t));
}

void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt)
{
	memcpy(rsdp->signature, RSDP_SIG, 8);
	memcpy(rsdp->oem_id, OEM_ID, 6);
	
	rsdp->length		= sizeof(acpi_rsdp_t);
	rsdp->rsdt_address	= (u32)rsdt;
	rsdp->checksum		= acpi_checksum((void *)rsdp, 20);
	rsdp->ext_checksum	= acpi_checksum((void *)rsdp, sizeof(acpi_rsdp_t));
}


