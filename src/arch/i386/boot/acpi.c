/*
 * LinuxBIOS ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 * (C) 2004 SUSE LINUX AG
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>

#define RSDP_SIG              "RSD PTR "  /* RSDT Pointer signature */
#define RSDP_NAME             "RSDP"

#define RSDT_NAME             "RSDT"
#define HPET_NAME             "HPET"
#define MADT_NAME             "APIC"

#define RSDT_TABLE            "RSDT    "
#define HPET_TABLE            "AMD64   "
#define MADT_TABLE            "MADT    "

#define OEM_ID                "LXBIOS"
#define ASLC                  "NONE"

static u8 acpi_checksum(u8 *table, u32 length)
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

static void acpi_add_table(acpi_rsdt_t *rsdt, void *table)
{
	int i;

	for (i=0; i<8; i++) {
		if(rsdt->entry[i]==0) {
			rsdt->entry[i]=(u32)table;
			/* fix checksum */
			/* hope this won't get optimized away */
			rsdt->header.checksum=0;
			rsdt->header.checksum=acpi_checksum((u8 *)rsdt,
					rsdt->header.length);
			
			printk_debug("ACPI: added table %d/8\n",i+1);
			return;
		}
	}

	printk_warning("ACPI: could not ACPI table. failed.\n");
}


static int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic)
{
	lapic->type=0;
	lapic->length=sizeof(acpi_madt_lapic_t);
	lapic->flags=1;
	
	lapic->processor_id=cpu;
	lapic->apic_id=apic;
	
	return(lapic->length);
}

static int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr) 
{
	ioapic->type=1;
	ioapic->length=sizeof(acpi_madt_ioapic_t);
	ioapic->reserved=0x00;
	ioapic->gsi_base=0x00000000;
	
	ioapic->ioapic_id=id;
	ioapic->ioapic_addr=addr;
	
	return(ioapic->length);
}

static int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
		u8 bus, u8 source, u32 gsirq, u16 flags)
{
	irqoverride->type=2;
	irqoverride->length=sizeof(acpi_madt_irqoverride_t);
	irqoverride->flags=0x0001;
	
	irqoverride->bus=bus;
	irqoverride->source=source;
	irqoverride->gsirq=gsirq;
	irqoverride->flags=flags;
	
	return(irqoverride->length);
}

static int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
		u16 flags, u8 lint)
{
	lapic_nmi->type=4;
	lapic_nmi->length=sizeof(acpi_madt_lapic_nmi_t);
	
	lapic_nmi->flags=flags;
	lapic_nmi->processor_id=cpu;
	lapic_nmi->lint=lint;
	
	return(lapic_nmi->length);
}

static void acpi_create_madt(acpi_madt_t *madt)
{
#define LOCAL_APIC_ADDR	0xfee00000ULL
#define IO_APIC_ADDR	0xfec00000UL
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

	/* recalculate length */
	header->length= current - (unsigned long)madt;
	
	header->checksum	= acpi_checksum((void *)madt, header->length);
}


static void acpi_create_hpet(acpi_hpet_t *hpet)
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

static void acpi_write_rsdt(acpi_rsdt_t *rsdt)
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

static void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt)
{
	memcpy(rsdp->signature, RSDP_SIG, 8);
	memcpy(rsdp->oem_id, OEM_ID, 6);
	
	rsdp->length		= sizeof(acpi_rsdp_t);
	rsdp->rsdt_address	= (u32)rsdt;
	rsdp->checksum		= acpi_checksum((void *)rsdp, 20);
	rsdp->ext_checksum	= acpi_checksum((void *)rsdp, sizeof(acpi_rsdp_t));
	
}

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	
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
#define HAVE_ACPI_HPET
#ifdef HAVE_ACPI_HPET
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


#endif
	printk_info("ACPI: done.\n");
	return current;
}

