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
#define RSDT_TABLE            "  RSDT  "
#define HPET_TABLE            "AMD64   "

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
#endif
	printk_info("ACPI: done.\n");
	return current;
}

