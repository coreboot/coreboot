/*
 * This file is part of the coreboot project.
 *
 * coreboot ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 *
 * Copyright (C) 2004 SUSE LINUX AG
 * Copyright (C) 2005-2009 coresystems GmbH
 *
 * ACPI FADT, FACS, and DSDT table support added by
 * Nick Barker <nick.barker9@btinternet.com>, and those portions
 * Copyright (C) 2004 Nick Barker
 *
 * Copyright (C) 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
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
#include <cpu/x86/lapic_def.h>
#include <cpu/cpu.h>
#if CONFIG_COLLECT_TIMESTAMPS
#include <timestamp.h>
#endif
#include <romstage_handoff.h>

/* FIXME: Kconfig doesn't support overridable defaults :-( */
#ifndef CONFIG_HPET_MIN_TICKS
#define CONFIG_HPET_MIN_TICKS 0x1000
#endif

u8 acpi_checksum(u8 *table, u32 length)
{
	u8 ret = 0;
	while (length--) {
		ret += *table;
		table++;
	}
	return -ret;
}

/**
 * Add an ACPI table to the RSDT (and XSDT) structure, recalculate length
 * and checksum.
 */
void acpi_add_table(acpi_rsdp_t *rsdp, void *table)
{
	int i, entries_num;
	acpi_rsdt_t *rsdt;
	acpi_xsdt_t *xsdt = NULL;

	/* The RSDT is mandatory... */
	rsdt = (acpi_rsdt_t *)rsdp->rsdt_address;

	/* ...while the XSDT is not. */
	if (rsdp->xsdt_address)
		xsdt = (acpi_xsdt_t *)((u32)rsdp->xsdt_address);

	/* This should always be MAX_ACPI_TABLES. */
	entries_num = ARRAY_SIZE(rsdt->entry);

	for (i = 0; i < entries_num; i++) {
		if (rsdt->entry[i] == 0)
			break;
	}

	if (i >= entries_num) {
		printk(BIOS_ERR, "ACPI: Error: Could not add ACPI table, "
			"too many tables.\n");
		return;
	}

	/* Add table to the RSDT. */
	rsdt->entry[i] = (u32)table;

	/* Fix RSDT length or the kernel will assume invalid entries. */
	rsdt->header.length = sizeof(acpi_header_t) + (sizeof(u32) * (i + 1));

	/* Re-calculate checksum. */
	rsdt->header.checksum = 0; /* Hope this won't get optimized away */
	rsdt->header.checksum = acpi_checksum((u8 *)rsdt, rsdt->header.length);

	/*
	 * And now the same thing for the XSDT. We use the same index as for
	 * now we want the XSDT and RSDT to always be in sync in coreboot.
	 */
	if (xsdt) {
		/* Add table to the XSDT. */
		xsdt->entry[i] = (u64)(u32)table;

		/* Fix XSDT length. */
		xsdt->header.length = sizeof(acpi_header_t) +
					(sizeof(u64) * (i + 1));

		/* Re-calculate checksum. */
		xsdt->header.checksum = 0;
		xsdt->header.checksum = acpi_checksum((u8 *)xsdt,
							xsdt->header.length);
	}

	printk(BIOS_DEBUG, "ACPI: added table %d/%d, length now %d\n",
		i + 1, entries_num, rsdt->header.length);
}

int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u32 base,
				u16 seg_nr, u8 start, u8 end)
{
	memset(mmconfig, 0, sizeof(*mmconfig));
	mmconfig->base_address = base;
	mmconfig->base_reserved = 0;
	mmconfig->pci_segment_group_number = seg_nr;
	mmconfig->start_bus_number = start;
	mmconfig->end_bus_number = end;

	return sizeof(acpi_mcfg_mmconfig_t);
}

int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic)
{
	lapic->type = 0; /* Local APIC structure */
	lapic->length = sizeof(acpi_madt_lapic_t);
	lapic->flags = (1 << 0); /* Processor/LAPIC enabled */
	lapic->processor_id = cpu;
	lapic->apic_id = apic;

	return lapic->length;
}

unsigned long acpi_create_madt_lapics(unsigned long current)
{
	struct device *cpu;
	int index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
			(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current,
				index, cpu->path.apic.apic_id);
		index++;
	}

	return current;
}

int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,
				u32 gsi_base)
{
	ioapic->type = 1; /* I/O APIC structure */
	ioapic->length = sizeof(acpi_madt_ioapic_t);
	ioapic->reserved = 0x00;
	ioapic->gsi_base = gsi_base;
	ioapic->ioapic_id = id;
	ioapic->ioapic_addr = addr;

	return ioapic->length;
}

int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
		u8 bus, u8 source, u32 gsirq, u16 flags)
{
	irqoverride->type = 2; /* Interrupt source override */
	irqoverride->length = sizeof(acpi_madt_irqoverride_t);
	irqoverride->bus = bus;
	irqoverride->source = source;
	irqoverride->gsirq = gsirq;
	irqoverride->flags = flags;

	return irqoverride->length;
}

int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
				u16 flags, u8 lint)
{
	lapic_nmi->type = 4; /* Local APIC NMI structure */
	lapic_nmi->length = sizeof(acpi_madt_lapic_nmi_t);
	lapic_nmi->flags = flags;
	lapic_nmi->processor_id = cpu;
	lapic_nmi->lint = lint;

	return lapic_nmi->length;
}

void acpi_create_madt(acpi_madt_t *madt)
{
	acpi_header_t *header = &(madt->header);
	unsigned long current = (unsigned long)madt + sizeof(acpi_madt_t);

	memset((void *)madt, 0, sizeof(acpi_madt_t));

	/* Fill out header fields. */
	memcpy(header->signature, "APIC", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_madt_t);
	header->revision = 1; /* ACPI 1.0/2.0: 1, ACPI 3.0: 2, ACPI 4.0: 3 */

	madt->lapic_addr = LOCAL_APIC_ADDR;
	madt->flags = 0x1; /* PCAT_COMPAT */

	current = acpi_fill_madt(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)madt;

	header->checksum = acpi_checksum((void *)madt, header->length);
}

/* MCFG is defined in the PCI Firmware Specification 3.0. */
void acpi_create_mcfg(acpi_mcfg_t *mcfg)
{
	acpi_header_t *header = &(mcfg->header);
	unsigned long current = (unsigned long)mcfg + sizeof(acpi_mcfg_t);

	memset((void *)mcfg, 0, sizeof(acpi_mcfg_t));

	/* Fill out header fields. */
	memcpy(header->signature, "MCFG", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_mcfg_t);
	header->revision = 1;

	current = acpi_fill_mcfg(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)mcfg;
	header->checksum = acpi_checksum((void *)mcfg, header->length);
}

#if !IS_ENABLED(CONFIG_PER_DEVICE_ACPI_TABLES)
/*
 * This can be overridden by platform ACPI setup code, if it calls
 * acpi_create_ssdt_generator().
 */
unsigned long __attribute__((weak)) acpi_fill_ssdt_generator(
	unsigned long current, const char *oem_table_id)
{
	return current;
}
#endif

void acpi_create_ssdt_generator(acpi_header_t *ssdt, const char *oem_table_id)
{
	unsigned long current = (unsigned long)ssdt + sizeof(acpi_header_t);

	memset((void *)ssdt, 0, sizeof(acpi_header_t));

	memcpy(&ssdt->signature, "SSDT", 4);
	ssdt->revision = 2; /* ACPI 1.0/2.0: ?, ACPI 3.0/4.0: 2 */
	memcpy(&ssdt->oem_id, OEM_ID, 6);
	memcpy(&ssdt->oem_table_id, oem_table_id, 8);
	ssdt->oem_revision = 42;
	memcpy(&ssdt->asl_compiler_id, ASLC, 4);
	ssdt->asl_compiler_revision = 42;
	ssdt->length = sizeof(acpi_header_t);

	acpigen_set_current((char *) current);
	{
#if IS_ENABLED(CONFIG_PER_DEVICE_ACPI_TABLES)
		struct device *dev;
		for (dev = all_devices; dev; dev = dev->next)
			if (dev->ops && dev->ops->acpi_fill_ssdt_generator) {
				dev->ops->acpi_fill_ssdt_generator();
			}
		current = (unsigned long) acpigen_get_current();
#else
		current = acpi_fill_ssdt_generator(current, oem_table_id);
#endif
	}

	/* (Re)calculate length and checksum. */
	ssdt->length = current - (unsigned long)ssdt;
	ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);
}

int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic)
{
	memset((void *)lapic, 0, sizeof(acpi_srat_lapic_t));

	lapic->type = 0; /* Processor local APIC/SAPIC affinity structure */
	lapic->length = sizeof(acpi_srat_lapic_t);
	lapic->flags = (1 << 0); /* Enabled (the use of this structure). */
	lapic->proximity_domain_7_0 = node;
	/* TODO: proximity_domain_31_8, local SAPIC EID, clock domain. */
	lapic->apic_id = apic;

	return lapic->length;
}

int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek, u32 sizek,
				u32 flags)
{
	mem->type = 1; /* Memory affinity structure */
	mem->length = sizeof(acpi_srat_mem_t);
	mem->base_address_low = (basek << 10);
	mem->base_address_high = (basek >> (32 - 10));
	mem->length_low = (sizek << 10);
	mem->length_high = (sizek >> (32 - 10));
	mem->proximity_domain = node;
	mem->flags = flags;

	return mem->length;
}

/* http://www.microsoft.com/whdc/system/sysinternals/sratdwn.mspx */
void acpi_create_srat(acpi_srat_t *srat)
{
	acpi_header_t *header = &(srat->header);
	unsigned long current = (unsigned long)srat + sizeof(acpi_srat_t);

	memset((void *)srat, 0, sizeof(acpi_srat_t));

	/* Fill out header fields. */
	memcpy(header->signature, "SRAT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_srat_t);
	header->revision = 1; /* ACPI 1.0: N/A, 2.0: 1, 3.0: 2, 4.0: 3 */

	srat->resv = 1; /* Spec: Reserved to 1 for backwards compatibility. */

	current = acpi_fill_srat(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)srat;
	header->checksum = acpi_checksum((void *)srat, header->length);
}

void acpi_create_dmar(acpi_dmar_t *dmar,
		      unsigned long (*acpi_fill_dmar) (unsigned long))
{
	acpi_header_t *header = &(dmar->header);
	unsigned long current = (unsigned long)dmar + sizeof(acpi_dmar_t);

	memset((void *)dmar, 0, sizeof(acpi_dmar_t));

	/* Fill out header fields. */
	memcpy(header->signature, "DMAR", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_dmar_t);
	header->revision = 1;

	dmar->host_address_width = 40 - 1; /* FIXME: == MTRR size? */
	dmar->flags = 0;

	current = acpi_fill_dmar(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)dmar;
	header->checksum = acpi_checksum((void *)dmar, header->length);
}

unsigned long acpi_create_dmar_drhd(unsigned long current, u8 flags,
	u16 segment, u32 bar)
{
	dmar_entry_t *drhd = (dmar_entry_t *)current;
	memset(drhd, 0, sizeof(*drhd));
	drhd->type = DMAR_DRHD;
	drhd->length = sizeof(*drhd); /* will be fixed up later */
	drhd->flags = flags;
	drhd->segment = segment;
	drhd->bar = bar;

	return drhd->length;
}

void acpi_dmar_drhd_fixup(unsigned long base, unsigned long current)
{
	dmar_entry_t *drhd = (dmar_entry_t *)base;
	drhd->length = current - base;
}

unsigned long acpi_create_dmar_drhd_ds_pci(unsigned long current, u8 segment,
	u8 dev, u8 fn)
{
	dev_scope_t *ds = (dev_scope_t *)current;
	memset(ds, 0, sizeof(*ds));
	ds->type = SCOPE_PCI_ENDPOINT;
	ds->length = sizeof(*ds) + 2; /* we don't support longer paths yet */
	ds->start_bus = segment;
	ds->path[0].dev = dev;
	ds->path[0].fn = fn;

	return ds->length;
}

/* http://h21007.www2.hp.com/portal/download/files/unprot/Itanium/slit.pdf */
void acpi_create_slit(acpi_slit_t *slit)
{
	acpi_header_t *header = &(slit->header);
	unsigned long current = (unsigned long)slit + sizeof(acpi_slit_t);

	memset((void *)slit, 0, sizeof(acpi_slit_t));

	/* Fill out header fields. */
	memcpy(header->signature, "SLIT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_slit_t);
	header->revision = 1; /* ACPI 1.0: N/A, ACPI 2.0/3.0/4.0: 1 */

	current = acpi_fill_slit(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)slit;
	header->checksum = acpi_checksum((void *)slit, header->length);
}

/* http://www.intel.com/hardwaredesign/hpetspec_1.pdf */
void acpi_create_hpet(acpi_hpet_t *hpet)
{
	acpi_header_t *header = &(hpet->header);
	acpi_addr_t *addr = &(hpet->addr);

	memset((void *)hpet, 0, sizeof(acpi_hpet_t));

	/* Fill out header fields. */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_hpet_t);
	header->revision = 1; /* Currently 1. Table added in ACPI 2.0. */

	/* Fill out HPET address. */
	addr->space_id = 0; /* Memory */
	addr->bit_width = 64;
	addr->bit_offset = 0;
	addr->addrl = CONFIG_HPET_ADDRESS & 0xffffffff;
	addr->addrh = ((unsigned long long)CONFIG_HPET_ADDRESS) >> 32;

	hpet->id = *(unsigned int*)CONFIG_HPET_ADDRESS;
	hpet->number = 0;
	hpet->min_tick = CONFIG_HPET_MIN_TICKS;

	header->checksum = acpi_checksum((void *)hpet, sizeof(acpi_hpet_t));
}

unsigned long acpi_write_hpet(unsigned long current, acpi_rsdp_t *rsdp)
{
	acpi_hpet_t *hpet;

	/*
	 * We explicitly add these tables later on:
	 */
	printk(BIOS_DEBUG, "ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	current = ALIGN(current, 16);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	return current;
}

void acpi_create_facs(acpi_facs_t *facs)
{
	memset((void *)facs, 0, sizeof(acpi_facs_t));

	memcpy(facs->signature, "FACS", 4);
	facs->length = sizeof(acpi_facs_t);
	facs->hardware_signature = 0;
	facs->firmware_waking_vector = 0;
	facs->global_lock = 0;
	facs->flags = 0;
	facs->x_firmware_waking_vector_l = 0;
	facs->x_firmware_waking_vector_h = 0;
	facs->version = 1; /* ACPI 1.0: 0, ACPI 2.0/3.0: 1, ACPI 4.0: 2 */
}

void acpi_write_rsdt(acpi_rsdt_t *rsdt)
{
	acpi_header_t *header = &(rsdt->header);

	/* Fill out header fields. */
	memcpy(header->signature, "RSDT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_rsdt_t);
	header->revision = 1; /* ACPI 1.0/2.0/3.0/4.0: 1 */

	/* Entries are filled in later, we come with an empty set. */

	/* Fix checksum. */
	header->checksum = acpi_checksum((void *)rsdt, sizeof(acpi_rsdt_t));
}

void acpi_write_xsdt(acpi_xsdt_t *xsdt)
{
	acpi_header_t *header = &(xsdt->header);

	/* Fill out header fields. */
	memcpy(header->signature, "XSDT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_xsdt_t);
	header->revision = 1; /* ACPI 1.0: N/A, 2.0/3.0/4.0: 1 */

	/* Entries are filled in later, we come with an empty set. */

	/* Fix checksum. */
	header->checksum = acpi_checksum((void *)xsdt, sizeof(acpi_xsdt_t));
}

void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt, acpi_xsdt_t *xsdt)
{
	memset(rsdp, 0, sizeof(acpi_rsdp_t));

	memcpy(rsdp->signature, RSDP_SIG, 8);
	memcpy(rsdp->oem_id, OEM_ID, 6);

	rsdp->length = sizeof(acpi_rsdp_t);
	rsdp->rsdt_address = (u32)rsdt;

	/*
	 * Revision: ACPI 1.0: 0, ACPI 2.0/3.0/4.0: 2.
	 *
	 * Some OSes expect an XSDT to be present for RSD PTR revisions >= 2.
	 * If we don't have an ACPI XSDT, force ACPI 1.0 (and thus RSD PTR
	 * revision 0).
	 */
	if (xsdt == NULL) {
		rsdp->revision = 0;
	} else {
		rsdp->xsdt_address = (u64)(u32)xsdt;
		rsdp->revision = 2;
	}

	/* Calculate checksums. */
	rsdp->checksum = acpi_checksum((void *)rsdp, 20);
	rsdp->ext_checksum = acpi_checksum((void *)rsdp, sizeof(acpi_rsdp_t));
}

unsigned long __attribute__((weak)) acpi_fill_hest(acpi_hest_t *hest)
{
	return (unsigned long)hest;
}

unsigned long acpi_create_hest_error_source(acpi_hest_t *hest, acpi_hest_esd_t *esd, u16 type, void *data, u16 data_len)
{
	acpi_header_t *header = &(hest->header);
	acpi_hest_hen_t *hen;
	void *pos;
	u16 len;

	pos = esd;
	memset(pos, 0, sizeof(acpi_hest_esd_t));
	len = 0;
	esd->type = type;		/* MCE */
	esd->source_id = hest->error_source_count;
	esd->flags = 0;		/* FIRMWARE_FIRST */
	esd->enabled = 1;
	esd->prealloc_erecords = 1;
	esd->max_section_per_record = 0x1;

	len += sizeof(acpi_hest_esd_t);
	pos = esd + 1;

	switch (type) {
	case 0:			/* MCE */
		break;
	case 1:			/* CMC */
		hen = (acpi_hest_hen_t *) (pos);
		memset(pos, 0, sizeof(acpi_hest_hen_t));
		hen->type = 3;		/* SCI? */
		hen->length = sizeof(acpi_hest_hen_t);
		hen->conf_we = 0;		/* Configuration Write Enable. */
		hen->poll_interval = 0;
		hen->vector = 0;
		hen->sw2poll_threshold_val = 0;
		hen->sw2poll_threshold_win = 0;
		hen->error_threshold_val = 0;
		hen->error_threshold_win = 0;
		len += sizeof(acpi_hest_hen_t);
		pos = hen + 1;
		break;
	case 2:			/* NMI */
	case 6:			/* AER Root Port */
	case 7:			/* AER Endpoint */
	case 8:			/* AER Bridge */
	case 9:			/* Generic Hardware Error Source. */
		/* TODO: */
		break;
	default:
		printk(BIOS_DEBUG, "Invalid type of Error Source.");
		break;
	}
	hest->error_source_count ++;

	memcpy(pos, data, data_len);
	len += data_len;
	header->length += len;

	return len;
}

/* ACPI 4.0 */
void acpi_write_hest(acpi_hest_t *hest)
{
	acpi_header_t *header = &(hest->header);

	memset(hest, 0, sizeof(acpi_hest_t));

	memcpy(header->signature, "HEST", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->length += sizeof(acpi_hest_t);
	header->revision = 1;

	acpi_fill_hest(hest);

	/* Calculate checksums. */
	header->checksum = acpi_checksum((void *)hest, header->length);
}

#if IS_ENABLED(CONFIG_COMMON_FADT)
void acpi_create_fadt(acpi_fadt_t *fadt,acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = 4;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->dsdt = (unsigned long) dsdt;

	fadt->x_firmware_ctl_l = (unsigned long)facs;
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (unsigned long)dsdt;
	fadt->x_dsdt_h = 0;

	if(IS_ENABLED(CONFIG_SYSTEM_TYPE_LAPTOP)) {
		fadt->preferred_pm_profile = PM_MOBILE;
	} else {
		fadt->preferred_pm_profile = PM_DESKTOP;
	}

	acpi_fill_fadt(fadt);

	header->checksum =
	    acpi_checksum((void *) fadt, header->length);
}
#endif

#if IS_ENABLED(CONFIG_PER_DEVICE_ACPI_TABLES)

extern const unsigned char AmlCode[];

unsigned long __attribute__ ((weak)) fw_cfg_acpi_tables(unsigned long start)
{
	return 0;
}

#define ALIGN_CURRENT current = (ALIGN(current, 16))
unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_xsdt_t *xsdt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
#if CONFIG_HAVE_ACPI_SLIC
	acpi_header_t *slic;
#endif
	acpi_header_t *ssdt;
	acpi_header_t *dsdt;
	acpi_mcfg_t *mcfg;
	acpi_madt_t *madt;
	struct device *dev;
	unsigned long fw;

	current = start;

	/* Align ACPI tables to 16byte */
	ALIGN_CURRENT;

	fw = fw_cfg_acpi_tables(current);
	if (fw)
		return fw;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	ALIGN_CURRENT;
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);
	ALIGN_CURRENT;
	xsdt = (acpi_xsdt_t *) current;
	current += sizeof(acpi_xsdt_t);
	ALIGN_CURRENT;

	/* clear all table memory */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, xsdt);
	acpi_write_rsdt(rsdt);
	acpi_write_xsdt(xsdt);

	printk(BIOS_DEBUG, "ACPI:    * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	ALIGN_CURRENT;
	acpi_create_facs(facs);

	printk(BIOS_DEBUG, "ACPI:    * DSDT\n");
	dsdt = (acpi_header_t *) current;
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	if (dsdt->length >= sizeof(acpi_header_t)) {
		current += sizeof(acpi_header_t);

		acpigen_set_current((char *) current);
		for (dev = all_devices; dev; dev = dev->next)
			if (dev->ops && dev->ops->acpi_inject_dsdt_generator) {
				dev->ops->acpi_inject_dsdt_generator();
			}
		current = (unsigned long) acpigen_get_current();
		memcpy((char *)current,
		       (char *)&AmlCode + sizeof(acpi_header_t),
		       dsdt->length - sizeof(acpi_header_t));
		current += dsdt->length - sizeof(acpi_header_t);

		/* (Re)calculate length and checksum. */
		dsdt->length = current - (unsigned long)dsdt;
		dsdt->checksum = 0;
		dsdt->checksum = acpi_checksum((void *)dsdt, dsdt->length);
	}

	ALIGN_CURRENT;

	printk(BIOS_DEBUG, "ACPI:    * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	ALIGN_CURRENT;

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

#if CONFIG_HAVE_ACPI_SLIC
	printk(BIOS_DEBUG, "ACPI:     * SLIC\n");
	slic = (acpi_header_t *)current;
	current += acpi_create_slic(current);
	ALIGN_CURRENT;
	acpi_add_table(rsdp, slic);
#endif

	printk(BIOS_DEBUG, "ACPI:     * SSDT\n");
	ssdt = (acpi_header_t *)current;
	acpi_create_ssdt_generator(ssdt, ACPI_TABLE_CREATOR);
	if (ssdt->length > sizeof(acpi_header_t)) {
		current += ssdt->length;
		acpi_add_table(rsdp, ssdt);
		ALIGN_CURRENT;
	}

	printk(BIOS_DEBUG, "ACPI:    * MCFG\n");
	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	if (mcfg->header.length > sizeof(acpi_mcfg_t)) {
		current += mcfg->header.length;
		ALIGN_CURRENT;
		acpi_add_table(rsdp, mcfg);
	}

	printk(BIOS_DEBUG, "ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	if (madt->header.length > sizeof(acpi_madt_t)) {
		current+=madt->header.length;
		acpi_add_table(rsdp,madt);
	}
	ALIGN_CURRENT;

	printk(BIOS_DEBUG, "current = %lx\n", current);

	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->write_acpi_tables) {
			current = dev->ops->write_acpi_tables(current, rsdp);
			ALIGN_CURRENT;
		}
	}

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}
#endif

#if CONFIG_HAVE_ACPI_RESUME
void __attribute__((weak)) mainboard_suspend_resume(void)
{
}

void acpi_resume(void *wake_vec)
{
#if CONFIG_HAVE_SMI_HANDLER
	u32 *gnvs_address = cbmem_find(CBMEM_ID_ACPI_GNVS_PTR);

	/* Restore GNVS pointer in SMM if found */
	if (gnvs_address && *gnvs_address) {
		printk(BIOS_DEBUG, "Restore GNVS pointer to 0x%08x\n",
		       *gnvs_address);
		smm_setup_structures((void *)*gnvs_address, NULL, NULL);
	}
#endif

	/* Call mainboard resume handler first, if defined. */
	mainboard_suspend_resume();

	post_code(POST_OS_RESUME);
	acpi_jump_to_wakeup(wake_vec);
}

/* This is filled with acpi_is_wakeup() call early in ramstage. */
int acpi_slp_type = -1;

#if IS_ENABLED(CONFIG_EARLY_CBMEM_INIT)
int acpi_get_sleep_type(void)
{
	struct romstage_handoff *handoff;

	handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);

	if (handoff == NULL) {
		printk(BIOS_DEBUG, "Unknown boot method, assuming normal.\n");
		return 0;
	} else if (handoff->s3_resume) {
		printk(BIOS_DEBUG, "S3 Resume.\n");
		return 3;
	} else {
		printk(BIOS_DEBUG, "Normal boot.\n");
		return 0;
	}
}
#endif

static void acpi_handoff_wakeup(void)
{
	if (acpi_slp_type < 0)
		acpi_slp_type = acpi_get_sleep_type();
}

int acpi_is_wakeup(void)
{
	acpi_handoff_wakeup();
	/* Both resume from S2 and resume from S3 restart at CPU reset */
	return (acpi_slp_type == 3 || acpi_slp_type == 2);
}

int acpi_is_wakeup_s3(void)
{
	acpi_handoff_wakeup();
	return (acpi_slp_type == 3);
}

void acpi_fail_wakeup(void)
{
	if (acpi_slp_type == 3 || acpi_slp_type == 2)
		acpi_slp_type = 0;
}

void acpi_prepare_resume_backup(void)
{
	if (!acpi_s3_resume_allowed())
		return;

	/* Let's prepare the ACPI S3 Resume area now already, so we can rely on
	 * it being there during reboot time. We don't need the pointer, nor
	 * the result right now. If it fails, ACPI resume will be disabled.
	 */

	if (HIGH_MEMORY_SAVE)
		cbmem_add(CBMEM_ID_RESUME, HIGH_MEMORY_SAVE);

	if (HIGH_MEMORY_SCRATCH)
		cbmem_add(CBMEM_ID_RESUME_SCRATCH, HIGH_MEMORY_SCRATCH);
}

static acpi_rsdp_t *valid_rsdp(acpi_rsdp_t *rsdp)
{
	if (strncmp((char *)rsdp, RSDP_SIG, sizeof(RSDP_SIG) - 1) != 0)
		return NULL;

	printk(BIOS_DEBUG, "Looking on %p for valid checksum\n", rsdp);

	if (acpi_checksum((void *)rsdp, 20) != 0)
		return NULL;
	printk(BIOS_DEBUG, "Checksum 1 passed\n");

	if ((rsdp->revision > 1) && (acpi_checksum((void *)rsdp,
						rsdp->length) != 0))
		return NULL;
	printk(BIOS_DEBUG, "Checksum 2 passed all OK\n");

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
	acpi_fadt_t *fadt = NULL;
	void *wake_vec;
	int i;

	rsdp = NULL;

	if (!acpi_is_wakeup())
		return NULL;

	printk(BIOS_DEBUG, "Trying to find the wakeup vector...\n");

	/* Find RSDP. */
	for (p = (char *)0xe0000; p < (char *)0xfffff; p += 16) {
		if ((rsdp = valid_rsdp((acpi_rsdp_t *)p)))
			break;
	}

	if (rsdp == NULL)
		return NULL;

	printk(BIOS_DEBUG, "RSDP found at %p\n", rsdp);
	rsdt = (acpi_rsdt_t *) rsdp->rsdt_address;

	end = (char *)rsdt + rsdt->header.length;
	printk(BIOS_DEBUG, "RSDT found at %p ends at %p\n", rsdt, end);

	for (i = 0; ((char *)&rsdt->entry[i]) < end; i++) {
		fadt = (acpi_fadt_t *)rsdt->entry[i];
		if (strncmp((char *)fadt, "FACP", 4) == 0)
			break;
		fadt = NULL;
	}

	if (fadt == NULL)
		return NULL;

	printk(BIOS_DEBUG, "FADT found at %p\n", fadt);
	facs = (acpi_facs_t *)fadt->firmware_ctrl;

	if (facs == NULL) {
		printk(BIOS_DEBUG, "No FACS found, wake up from S3 not "
		       "possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "FACS found at %p\n", facs);
	wake_vec = (void *)facs->firmware_waking_vector;
	printk(BIOS_DEBUG, "OS waking vector is %p\n", wake_vec);

	return wake_vec;
}

#if CONFIG_SMP
extern char *lowmem_backup;
extern char *lowmem_backup_ptr;
extern int lowmem_backup_size;
#endif

#define WAKEUP_BASE 0x600

void (*acpi_do_wakeup)(u32 vector, u32 backup_source, u32 backup_target,
       u32 backup_size) asmlinkage = (void *)WAKEUP_BASE;

extern unsigned char __wakeup;
extern unsigned int __wakeup_size;

void acpi_jump_to_wakeup(void *vector)
{
	u32 acpi_backup_memory = 0;

	if (HIGH_MEMORY_SAVE && acpi_s3_resume_allowed()) {
		acpi_backup_memory = (u32)cbmem_find(CBMEM_ID_RESUME);

		if (!acpi_backup_memory) {
			printk(BIOS_WARNING, "ACPI: Backup memory missing. "
				"No S3 resume.\n");
			return;
		}
	}

#if CONFIG_SMP
	// FIXME: This should go into the ACPI backup memory, too. No pork sausages.
	/*
	 * Just restore the SMP trampoline and continue with wakeup on
	 * assembly level.
	 */
	memcpy(lowmem_backup_ptr, lowmem_backup, lowmem_backup_size);
#endif

	/* Copy wakeup trampoline in place. */
	memcpy((void *)WAKEUP_BASE, &__wakeup, __wakeup_size);

#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_add_now(TS_ACPI_WAKE_JUMP);
#endif

	acpi_do_wakeup((u32)vector, acpi_backup_memory, CONFIG_RAMBASE,
		       HIGH_MEMORY_SAVE);
}
#endif

void acpi_save_gnvs(u32 gnvs_address)
{
	u32 *gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS_PTR, sizeof(*gnvs));
	if (gnvs)
		*gnvs = gnvs_address;
}
