/*
 * coreboot ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  Copyright (C) 2004 SUSE LINUX AG
 *  Copyright (C) 2005-2009 coresystems GmbH
 *
 * ACPI FADT, FACS, and DSDT table support added by 
 * Nick Barker <nick.barker9@btinternet.com>, and those portions
 *  (C) Copyright 2004 Nick Barker
 *
 * Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
 * 2005.9 yhlu add SRAT table generation
 */

/* 
 * Each system port implementing ACPI has to provide two functions:
 * 
 *   write_acpi_tables()
 *   acpi_dump_apics()
 *   
 * See Kontron 986LCD-M port for a good example of an ACPI implementation
 * in coreboot.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/pci.h>
#include <cbmem.h>

u8 acpi_checksum(u8 *table, u32 length)
{
	u8 ret=0;
	while (length--) {
		ret += *table;
		table++;
	}
	return -ret;
}

/**
 * Add an ACPI table to the RSDT (and XSDT) structure, recalculate length and checksum
 */

void acpi_add_table(acpi_rsdp_t *rsdp, void *table)
{
	int i, entries_num;
	acpi_rsdt_t *rsdt;
	acpi_xsdt_t *xsdt = NULL;

	/* The RSDT is mandatory ... */
	rsdt = (acpi_rsdt_t *)rsdp->rsdt_address;

	/* ... while the XSDT is not */
	if (rsdp->xsdt_address) {
		xsdt = (acpi_xsdt_t *)((u32)rsdp->xsdt_address);
	}
	
	/* This should always be MAX_ACPI_TABLES */
	entries_num = ARRAY_SIZE(rsdt->entry);
	
	for (i = 0; i < entries_num; i++) {
		if(rsdt->entry[i] == 0)
			break;
	}

	if (i >= entries_num) {
		printk_err("ACPI: Error: Could not add ACPI table, too many tables.\n");
		return;
	}

	/* Add table to the RSDT */
	rsdt->entry[i] = (u32)table;

	/* Fix RSDT length or the kernel will assume invalid entries */
	rsdt->header.length = sizeof(acpi_header_t) + (sizeof(u32) * (i+1));

	/* Re-calculate checksum */
	rsdt->header.checksum = 0; /* Hope this won't get optimized away */
	rsdt->header.checksum = acpi_checksum((u8 *)rsdt,
			rsdt->header.length);

	/* And now the same thing for the XSDT. We use the same index as for
	 * now we want the XSDT and RSDT to always be in sync in coreboot.
	 */
	if (xsdt) {
		/* Add table to the XSDT */
		xsdt->entry[i]=(u64)(u32)table;

		/* Fix XSDT length */
		xsdt->header.length = sizeof(acpi_header_t) +
			(sizeof(u64) * (i+1));

		/* Re-calculate checksum */
		xsdt->header.checksum=0;
		xsdt->header.checksum=acpi_checksum((u8 *)xsdt,
				xsdt->header.length);
	}

	printk_debug("ACPI: added table %d/%d Length now %d\n",
			i+1, entries_num, rsdt->header.length);
}

int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u32 base, u16 seg_nr, u8 start, u8 end)
{
	mmconfig->base_address = base;
	mmconfig->base_reserved = 0;
	mmconfig->pci_segment_group_number = seg_nr;
	mmconfig->start_bus_number = start;
	mmconfig->end_bus_number = end;
	return (sizeof(acpi_mcfg_mmconfig_t));
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

unsigned long acpi_create_madt_lapics(unsigned long current)
{
	device_t cpu;
	int cpu_index = 0;

	for(cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		   (cpu->bus->dev->path.type != DEVICE_PATH_APIC_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, cpu_index, cpu->path.apic.apic_id);
		cpu_index++;
	}
	return current;
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
	memcpy(header->signature, "APIC", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
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

void acpi_create_mcfg(acpi_mcfg_t *mcfg)
{

	acpi_header_t *header=&(mcfg->header);
	unsigned long current=(unsigned long)mcfg+sizeof(acpi_mcfg_t);
	
	memset((void *)mcfg, 0, sizeof(acpi_mcfg_t));
	
	/* fill out header fields */
	memcpy(header->signature, "MCFG", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	
	header->length = sizeof(acpi_mcfg_t);
	header->revision = 1;

	current = acpi_fill_mcfg(current);
	
	/* recalculate length */
	header->length= current - (unsigned long)mcfg;
	
	header->checksum	= acpi_checksum((void *)mcfg, header->length);
}

/* this can be overriden by platform ACPI setup code,
 * if it calls acpi_create_ssdt_generator
 */
unsigned long __attribute__((weak)) acpi_fill_ssdt_generator(unsigned long current,
						    const char *oem_table_id) {
	return current;
}

void acpi_create_ssdt_generator(acpi_header_t *ssdt, const char *oem_table_id)
{
	unsigned long current=(unsigned long)ssdt+sizeof(acpi_header_t);
	memset((void *)ssdt, 0, sizeof(acpi_header_t));
	memcpy(&ssdt->signature, "SSDT", 4);
	ssdt->revision = 2;
	memcpy(&ssdt->oem_id, OEM_ID, 6);
	memcpy(&ssdt->oem_table_id, oem_table_id, 8);
	ssdt->oem_revision = 42;
	memcpy(&ssdt->asl_compiler_id, "CORE", 4);
	ssdt->asl_compiler_revision = 42;
	ssdt->length = sizeof(acpi_header_t);

	acpigen_set_current((char *) current);
	current = acpi_fill_ssdt_generator(current, oem_table_id);

	/* recalculate length */
	ssdt->length = current - (unsigned long)ssdt;
	ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);
}

int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic)
{
	memset((void *)lapic, 0, sizeof(acpi_srat_lapic_t));
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
        memcpy(header->signature, "SRAT", 4);
        memcpy(header->oem_id, OEM_ID, 6);
        memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
        memcpy(header->asl_compiler_id, ASLC, 4);

        header->length = sizeof(acpi_srat_t);
        header->revision = 1;

        srat->resv     = 0x1; /* BACK COMP */

        current = acpi_fill_srat(current);

        /* recalculate length */
        header->length= current - (unsigned long)srat;

        header->checksum        = acpi_checksum((void *)srat, header->length);
}

void acpi_create_slit(acpi_slit_t *slit)
{

        acpi_header_t *header=&(slit->header);
        unsigned long current=(unsigned long)slit+sizeof(acpi_slit_t);

        memset((void *)slit, 0, sizeof(acpi_slit_t));

        /* fill out header fields */
        memcpy(header->signature, "SLIT", 4);
        memcpy(header->oem_id, OEM_ID, 6);
        memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
        memcpy(header->asl_compiler_id, ASLC, 4);

        header->length = sizeof(acpi_slit_t);
        header->revision = 1;

        current = acpi_fill_slit(current);

        /* recalculate length */
        header->length= current - (unsigned long)slit;

        header->checksum        = acpi_checksum((void *)slit, header->length);
}

void acpi_create_hpet(acpi_hpet_t *hpet)
{
#define HPET_ADDR  0xfed00000ULL
	acpi_header_t *header=&(hpet->header);
	acpi_addr_t *addr=&(hpet->addr);
	
	memset((void *)hpet, 0, sizeof(acpi_hpet_t));
	
	/* fill out header fields */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
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

	memcpy(facs->signature, "FACS", 4);
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
	memcpy(header->signature, "RSDT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	
	header->length = sizeof(acpi_rsdt_t);
	header->revision = 1;
	
	/* fill out entries */

	// entries are filled in later, we come with an empty set.
	
	/* fix checksum */
	
	header->checksum = acpi_checksum((void *)rsdt, sizeof(acpi_rsdt_t));
}

void acpi_write_xsdt(acpi_xsdt_t *xsdt)
{ 
	acpi_header_t *header=&(xsdt->header);
	
	/* fill out header fields */
	memcpy(header->signature, "XSDT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	
	header->length = sizeof(acpi_xsdt_t);
	header->revision = 1;
	
	/* fill out entries */

	// entries are filled in later, we come with an empty set.
	
	/* fix checksum */
	
	header->checksum = acpi_checksum((void *)xsdt, sizeof(acpi_xsdt_t));
}

void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt, acpi_xsdt_t *xsdt)
{
	memset(rsdp, 0, sizeof(acpi_rsdp_t));
	memcpy(rsdp->signature, RSDP_SIG, 8);
	memcpy(rsdp->oem_id, OEM_ID, 6);
	rsdp->length		= sizeof(acpi_rsdp_t);
	rsdp->rsdt_address	= (u32)rsdt;
	/* Some OSes expect an XSDT to be present for RSD PTR 
	 * revisions >= 2. If we don't have an ACPI XSDT, force
	 * ACPI 1.0 (and thus RSD PTR revision 0)
	 */
	if (xsdt == NULL) {
		rsdp->revision		= 0;
	} else {
		rsdp->xsdt_address	= (u64)(u32)xsdt;
		rsdp->revision		= 2;
	}
	rsdp->checksum		= acpi_checksum((void *)rsdp, 20);
	rsdp->ext_checksum	= acpi_checksum((void *)rsdp, sizeof(acpi_rsdp_t));
}

#if CONFIG_HAVE_ACPI_RESUME == 1
void suspend_resume(void)
{
	void *wake_vec;

#if 0
#if CONFIG_MEM_TRAIN_SEQ != 0
	#error "So far it works on AMD and CONFIG_MEM_TRAIN_SEQ == 0"
#endif

#if CONFIG_RAMBASE < 0x1F00000
	#error "For ACPI RESUME you need to have CONFIG_RAMBASE at least 31MB"
	#error "Chipset support (S3_NVRAM_EARLY and ACPI_IS_WAKEUP_EARLY functions and memory ctrl)"
	#error "And coreboot memory reserved in mainboard.c"
#endif
#endif
	/* if we happen to be resuming find wakeup vector and jump to OS */
	wake_vec = acpi_find_wakeup_vector();
	if (wake_vec)
		acpi_jump_to_wakeup(wake_vec);
}

/* this is to be filled by SB code - startup value what was found */
u8 acpi_slp_type = 0;

int acpi_is_wakeup(void)
{
	return (acpi_slp_type == 3);
}

static acpi_rsdp_t *valid_rsdp(acpi_rsdp_t *rsdp)
{
	if (strncmp((char *)rsdp, RSDP_SIG, sizeof(RSDP_SIG) - 1) != 0)
		return NULL;

	printk_debug("Looking on %p for valid checksum\n", rsdp);

	if (acpi_checksum((void *)rsdp, 20) != 0)
		return NULL;
	printk_debug("Checksum 1 passed\n");

	if ((rsdp->revision > 1) && (acpi_checksum((void *)rsdp,
						rsdp->length) != 0))
		return NULL;

	printk_debug("Checksum 2 passed all OK\n");

	return rsdp;
}

static acpi_rsdp_t *rsdp;

void *acpi_get_wakeup_rsdp(void)
{
	return rsdp;
}

void *acpi_find_wakeup_vector(void)
{
	char *p, *end;

	acpi_rsdt_t *rsdt;
	acpi_facs_t *facs;
	acpi_fadt_t *fadt;
	void  *wake_vec;
	int i;

	rsdp = NULL;

	if (!acpi_is_wakeup())
		return NULL;

	printk_debug("Trying to find the wakeup vector ...\n");

	/* find RSDP */
	for (p = (char *) 0xe0000; p <  (char *) 0xfffff; p+=16) {
		if ((rsdp = valid_rsdp((acpi_rsdp_t *) p)))
			break;
	}

	if (rsdp == NULL)
		return NULL;

	printk_debug("RSDP found at %p\n", rsdp);
	rsdt = (acpi_rsdt_t *) rsdp->rsdt_address;
	
	end = (char *) rsdt + rsdt->header.length;
	printk_debug("RSDT found at %p ends at %p\n", rsdt, end);

	for (i = 0; ((char *) &rsdt->entry[i]) < end; i++) {
		fadt = (acpi_fadt_t *) rsdt->entry[i];
		if (strncmp((char *)fadt, "FACP", 4) == 0)
			break;
		fadt = NULL;
	}

	if (fadt == NULL)
		return NULL;

	printk_debug("FADT found at %p\n", fadt);
	facs = (acpi_facs_t *)fadt->firmware_ctrl;

	if (facs == NULL) {
		printk_debug("No FACS found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk_debug("FACS found at %p\n", facs);
	wake_vec = (void *) facs->firmware_waking_vector;
	printk_debug("OS waking vector is %p\n", wake_vec);
	return wake_vec;
}

extern char *lowmem_backup;
extern char *lowmem_backup_ptr;
extern int lowmem_backup_size;

#define WAKEUP_BASE		0x600

void (*acpi_do_wakeup)(u32 vector, u32 backup_source, u32 backup_target, u32
		backup_size) __attribute__((regparm(0))) = (void *)WAKEUP_BASE;

extern unsigned char __wakeup, __wakeup_size;

void acpi_jump_to_wakeup(void *vector)
{
	u32 acpi_backup_memory = (u32) cbmem_find(CBMEM_ID_RESUME);

	if (!acpi_backup_memory) {
		printk(BIOS_WARNING, "ACPI: Backup memory missing. No S3 Resume.\n");
		return;
	}

	// FIXME this should go into the ACPI backup memory, too. No pork saussages.
	/* just restore the SMP trampoline and continue with wakeup on assembly level */
	memcpy(lowmem_backup_ptr, lowmem_backup, lowmem_backup_size);

	/* copy wakeup trampoline in place */
	memcpy(WAKEUP_BASE, &__wakeup, &__wakeup_size);

	acpi_do_wakeup((u32)vector, acpi_backup_memory, CONFIG_RAMBASE, HIGH_MEMORY_SAVE);
}
#endif
