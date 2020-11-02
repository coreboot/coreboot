/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * coreboot ACPI Table support
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
#include <acpi/acpi.h>
#include <acpi/acpi_ivrs.h>
#include <acpi/acpigen.h>
#include <device/pci.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <cpu/cpu.h>
#include <cbfs.h>
#include <version.h>
#include <commonlib/sort.h>

static acpi_rsdp_t *valid_rsdp(acpi_rsdp_t *rsdp);

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
	rsdt = (acpi_rsdt_t *)(uintptr_t)rsdp->rsdt_address;

	/* ...while the XSDT is not. */
	if (rsdp->xsdt_address)
		xsdt = (acpi_xsdt_t *)((uintptr_t)rsdp->xsdt_address);

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
	rsdt->entry[i] = (uintptr_t)table;

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
		xsdt->entry[i] = (u64)(uintptr_t)table;

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
	lapic->type = LOCAL_APIC; /* Local APIC structure */
	lapic->length = sizeof(acpi_madt_lapic_t);
	lapic->flags = (1 << 0); /* Processor/LAPIC enabled */
	lapic->processor_id = cpu;
	lapic->apic_id = apic;

	return lapic->length;
}

int acpi_create_madt_lx2apic(acpi_madt_lx2apic_t *lapic, u32 cpu, u32 apic)
{
	lapic->type = LOCAL_X2APIC; /* Local APIC structure */
	lapic->reserved = 0;
	lapic->length = sizeof(acpi_madt_lx2apic_t);
	lapic->flags = (1 << 0); /* Processor/LAPIC enabled */
	lapic->processor_id = cpu;
	lapic->x2apic_id = apic;

	return lapic->length;
}

unsigned long acpi_create_madt_lapics(unsigned long current)
{
	struct device *cpu;
	int index, apic_ids[CONFIG_MAX_CPUS] = {0}, num_cpus = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
			(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		if (num_cpus >= ARRAY_SIZE(apic_ids))
			break;
		apic_ids[num_cpus++] = cpu->path.apic.apic_id;
	}
	if (num_cpus > 1)
		bubblesort(apic_ids, num_cpus, NUM_ASCENDING);
	for (index = 0; index < num_cpus; index++) {
		if (apic_ids[index] < 0xff)
			current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current,
					index, apic_ids[index]);
		else
			current += acpi_create_madt_lx2apic((acpi_madt_lx2apic_t *)current,
					index, apic_ids[index]);
	}

	return current;
}

int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,
				u32 gsi_base)
{
	ioapic->type = IO_APIC; /* I/O APIC structure */
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
	irqoverride->type = IRQ_SOURCE_OVERRIDE; /* Interrupt source override */
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
	lapic_nmi->type = LOCAL_APIC_NMI; /* Local APIC NMI structure */
	lapic_nmi->length = sizeof(acpi_madt_lapic_nmi_t);
	lapic_nmi->flags = flags;
	lapic_nmi->processor_id = cpu;
	lapic_nmi->lint = lint;

	return lapic_nmi->length;
}

int acpi_create_madt_lx2apic_nmi(acpi_madt_lx2apic_nmi_t *lapic_nmi, u32 cpu,
				 u16 flags, u8 lint)
{
	lapic_nmi->type = LOCAL_X2APIC_NMI; /* Local APIC NMI structure */
	lapic_nmi->length = sizeof(acpi_madt_lx2apic_nmi_t);
	lapic_nmi->flags = flags;
	lapic_nmi->processor_id = cpu;
	lapic_nmi->lint = lint;
	lapic_nmi->reserved[0] = 0;
	lapic_nmi->reserved[1] = 0;
	lapic_nmi->reserved[2] = 0;

	return lapic_nmi->length;
}

__weak uintptr_t cpu_get_lapic_addr(void)
{
	/*
	 * If an architecture does not support LAPIC, this weak implementation returns LAPIC
	 * addr as 0.
	 */
	return 0;
}

void acpi_create_madt(acpi_madt_t *madt)
{
	acpi_header_t *header = &(madt->header);
	unsigned long current = (unsigned long)madt + sizeof(acpi_madt_t);

	memset((void *)madt, 0, sizeof(acpi_madt_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "APIC", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_madt_t);
	header->revision = get_acpi_table_revision(MADT);

	madt->lapic_addr = cpu_get_lapic_addr();
	if (CONFIG(ACPI_HAVE_PCAT_8259))
		madt->flags |= 1;

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

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "MCFG", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_mcfg_t);
	header->revision = get_acpi_table_revision(MCFG);

	current = acpi_fill_mcfg(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)mcfg;
	header->checksum = acpi_checksum((void *)mcfg, header->length);
}

static void *get_tcpa_log(u32 *size)
{
	const struct cbmem_entry *ce;
	const u32 tcpa_default_log_len = 0x10000;
	void *lasa;
	ce = cbmem_entry_find(CBMEM_ID_TCPA_TCG_LOG);
	if (ce) {
		lasa = cbmem_entry_start(ce);
		*size = cbmem_entry_size(ce);
		printk(BIOS_DEBUG, "TCPA log found at %p\n", lasa);
		return lasa;
	}
	lasa = cbmem_add(CBMEM_ID_TCPA_TCG_LOG, tcpa_default_log_len);
	if (!lasa) {
		printk(BIOS_ERR, "TCPA log creation failed\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "TCPA log created at %p\n", lasa);
	memset(lasa, 0, tcpa_default_log_len);

	*size = tcpa_default_log_len;
	return lasa;
}

static void acpi_create_tcpa(acpi_tcpa_t *tcpa)
{
	acpi_header_t *header = &(tcpa->header);
	u32 tcpa_log_len;
	void *lasa;

	memset((void *)tcpa, 0, sizeof(acpi_tcpa_t));

	lasa = get_tcpa_log(&tcpa_log_len);
	if (!lasa)
		return;

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "TCPA", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_tcpa_t);
	header->revision = get_acpi_table_revision(TCPA);

	tcpa->platform_class = 0;
	tcpa->laml = tcpa_log_len;
	tcpa->lasa = (uintptr_t) lasa;

	/* Calculate checksum. */
	header->checksum = acpi_checksum((void *)tcpa, header->length);
}

static void *get_tpm2_log(u32 *size)
{
	const struct cbmem_entry *ce;
	const u32 tpm2_default_log_len = 0x10000;
	void *lasa;
	ce = cbmem_entry_find(CBMEM_ID_TPM2_TCG_LOG);
	if (ce) {
		lasa = cbmem_entry_start(ce);
		*size = cbmem_entry_size(ce);
		printk(BIOS_DEBUG, "TPM2 log found at %p\n", lasa);
		return lasa;
	}
	lasa = cbmem_add(CBMEM_ID_TPM2_TCG_LOG, tpm2_default_log_len);
	if (!lasa) {
		printk(BIOS_ERR, "TPM2 log creation failed\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "TPM2 log created at %p\n", lasa);
	memset(lasa, 0, tpm2_default_log_len);

	*size = tpm2_default_log_len;
	return lasa;
}

static void acpi_create_tpm2(acpi_tpm2_t *tpm2)
{
	acpi_header_t *header = &(tpm2->header);
	u32 tpm2_log_len;
	void *lasa;

	memset((void *)tpm2, 0, sizeof(acpi_tpm2_t));

	/*
	 * Some payloads like SeaBIOS depend on log area to use TPM2.
	 * Get the memory size and address of TPM2 log area or initialize it.
	 */
	lasa = get_tpm2_log(&tpm2_log_len);
	if (!lasa)
		tpm2_log_len = 0;

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "TPM2", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_tpm2_t);
	header->revision = get_acpi_table_revision(TPM2);

	/* Hard to detect for coreboot. Just set it to 0 */
	tpm2->platform_class = 0;
	if (CONFIG(CRB_TPM)) {
		/* Must be set to 7 for CRB Support */
		tpm2->control_area = CONFIG_CRB_TPM_BASE_ADDRESS + 0x40;
		tpm2->start_method = 7;
	} else {
		/* Must be set to 0 for FIFO interface support */
		tpm2->control_area = 0;
		tpm2->start_method = 6;
	}
	memset(tpm2->msp, 0, sizeof(tpm2->msp));

	/* Fill the log area size and start address fields. */
	tpm2->laml = tpm2_log_len;
	tpm2->lasa = (uintptr_t) lasa;

	/* Calculate checksum. */
	header->checksum = acpi_checksum((void *)tpm2, header->length);
}

static void acpi_ssdt_write_cbtable(void)
{
	const struct cbmem_entry *cbtable;
	uintptr_t base;
	uint32_t size;

	cbtable = cbmem_entry_find(CBMEM_ID_CBTABLE);
	if (!cbtable)
		return;
	base = (uintptr_t)cbmem_entry_start(cbtable);
	size = cbmem_entry_size(cbtable);

	acpigen_write_device("CTBL");
	acpigen_write_coreboot_hid(COREBOOT_ACPI_ID_CBTABLE);
	acpigen_write_name_integer("_UID", 0);
	acpigen_write_STA(ACPI_STATUS_DEVICE_HIDDEN_ON);
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(0, base, size);
	acpigen_write_resourcetemplate_footer();
	acpigen_pop_len();
}

void acpi_create_ssdt_generator(acpi_header_t *ssdt, const char *oem_table_id)
{
	unsigned long current = (unsigned long)ssdt + sizeof(acpi_header_t);

	memset((void *)ssdt, 0, sizeof(acpi_header_t));

	memcpy(&ssdt->signature, "SSDT", 4);
	ssdt->revision = get_acpi_table_revision(SSDT);
	memcpy(&ssdt->oem_id, OEM_ID, 6);
	memcpy(&ssdt->oem_table_id, oem_table_id, 8);
	ssdt->oem_revision = 42;
	memcpy(&ssdt->asl_compiler_id, ASLC, 4);
	ssdt->asl_compiler_revision = asl_revision;
	ssdt->length = sizeof(acpi_header_t);

	acpigen_set_current((char *) current);

	/* Write object to declare coreboot tables */
	acpi_ssdt_write_cbtable();

	{
		struct device *dev;
		for (dev = all_devices; dev; dev = dev->next)
			if (dev->enabled && dev->ops && dev->ops->acpi_fill_ssdt)
				dev->ops->acpi_fill_ssdt(dev);
		current = (unsigned long) acpigen_get_current();
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
void acpi_create_srat(acpi_srat_t *srat,
		      unsigned long (*acpi_fill_srat)(unsigned long current))
{
	acpi_header_t *header = &(srat->header);
	unsigned long current = (unsigned long)srat + sizeof(acpi_srat_t);

	memset((void *)srat, 0, sizeof(acpi_srat_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "SRAT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_srat_t);
	header->revision = get_acpi_table_revision(SRAT);

	srat->resv = 1; /* Spec: Reserved to 1 for backwards compatibility. */

	current = acpi_fill_srat(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)srat;
	header->checksum = acpi_checksum((void *)srat, header->length);
}

void acpi_create_dmar(acpi_dmar_t *dmar, enum dmar_flags flags,
		      unsigned long (*acpi_fill_dmar)(unsigned long))
{
	acpi_header_t *header = &(dmar->header);
	unsigned long current = (unsigned long)dmar + sizeof(acpi_dmar_t);

	memset((void *)dmar, 0, sizeof(acpi_dmar_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "DMAR", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_dmar_t);
	header->revision = get_acpi_table_revision(DMAR);

	dmar->host_address_width = cpu_phys_address_size() - 1;
	dmar->flags = flags;

	current = acpi_fill_dmar(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)dmar;
	header->checksum = acpi_checksum((void *)dmar, header->length);
}

unsigned long acpi_create_dmar_drhd(unsigned long current, u8 flags,
	u16 segment, u64 bar)
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

unsigned long acpi_create_dmar_rmrr(unsigned long current, u16 segment,
				    u64 bar, u64 limit)
{
	dmar_rmrr_entry_t *rmrr = (dmar_rmrr_entry_t *)current;
	memset(rmrr, 0, sizeof(*rmrr));
	rmrr->type = DMAR_RMRR;
	rmrr->length = sizeof(*rmrr); /* will be fixed up later */
	rmrr->segment = segment;
	rmrr->bar = bar;
	rmrr->limit = limit;

	return rmrr->length;
}

unsigned long acpi_create_dmar_atsr(unsigned long current, u8 flags,
	u16 segment)
{
	dmar_atsr_entry_t *atsr = (dmar_atsr_entry_t *)current;
	memset(atsr, 0, sizeof(*atsr));
	atsr->type = DMAR_ATSR;
	atsr->length = sizeof(*atsr); /* will be fixed up later */
	atsr->flags = flags;
	atsr->segment = segment;

	return atsr->length;
}

unsigned long acpi_create_dmar_rhsa(unsigned long current, u64 base_addr,
	u32 proximity_domain)
{
	dmar_rhsa_entry_t *rhsa = (dmar_rhsa_entry_t *)current;
	memset(rhsa, 0, sizeof(*rhsa));
	rhsa->type = DMAR_RHSA;
	rhsa->length = sizeof(*rhsa);
	rhsa->base_address = base_addr;
	rhsa->proximity_domain = proximity_domain;

	return rhsa->length;
}

unsigned long acpi_create_dmar_andd(unsigned long current, u8 device_number,
	const char *device_name)
{
	dmar_andd_entry_t *andd = (dmar_andd_entry_t *)current;
	int andd_len = sizeof(dmar_andd_entry_t) + strlen(device_name) + 1;
	memset(andd, 0, andd_len);
	andd->type = DMAR_ANDD;
	andd->length = andd_len;
	andd->device_number = device_number;
	memcpy(&andd->device_name, device_name, strlen(device_name));

	return andd->length;
}

void acpi_dmar_drhd_fixup(unsigned long base, unsigned long current)
{
	dmar_entry_t *drhd = (dmar_entry_t *)base;
	drhd->length = current - base;
}

void acpi_dmar_rmrr_fixup(unsigned long base, unsigned long current)
{
	dmar_rmrr_entry_t *rmrr = (dmar_rmrr_entry_t *)base;
	rmrr->length = current - base;
}

void acpi_dmar_atsr_fixup(unsigned long base, unsigned long current)
{
	dmar_atsr_entry_t *atsr = (dmar_atsr_entry_t *)base;
	atsr->length = current - base;
}

static unsigned long acpi_create_dmar_ds(unsigned long current,
	enum dev_scope_type type, u8 enumeration_id, u8 bus, u8 dev, u8 fn)
{
	/* we don't support longer paths yet */
	const size_t dev_scope_length = sizeof(dev_scope_t) + 2;

	dev_scope_t *ds = (dev_scope_t *)current;
	memset(ds, 0, dev_scope_length);
	ds->type	= type;
	ds->length	= dev_scope_length;
	ds->enumeration	= enumeration_id;
	ds->start_bus	= bus;
	ds->path[0].dev	= dev;
	ds->path[0].fn	= fn;

	return ds->length;
}

unsigned long acpi_create_dmar_ds_pci_br(unsigned long current, u8 bus,
	u8 dev, u8 fn)
{
	return acpi_create_dmar_ds(current,
			SCOPE_PCI_SUB, 0, bus, dev, fn);
}

unsigned long acpi_create_dmar_ds_pci(unsigned long current, u8 bus,
	u8 dev, u8 fn)
{
	return acpi_create_dmar_ds(current,
			SCOPE_PCI_ENDPOINT, 0, bus, dev, fn);
}

unsigned long acpi_create_dmar_ds_ioapic(unsigned long current,
	u8 enumeration_id, u8 bus, u8 dev, u8 fn)
{
	return acpi_create_dmar_ds(current,
			SCOPE_IOAPIC, enumeration_id, bus, dev, fn);
}

unsigned long acpi_create_dmar_ds_msi_hpet(unsigned long current,
	u8 enumeration_id, u8 bus, u8 dev, u8 fn)
{
	return acpi_create_dmar_ds(current,
			SCOPE_MSI_HPET, enumeration_id, bus, dev, fn);
}

/* http://h21007.www2.hp.com/portal/download/files/unprot/Itanium/slit.pdf */
void acpi_create_slit(acpi_slit_t *slit,
		      unsigned long (*acpi_fill_slit)(unsigned long current))
{
	acpi_header_t *header = &(slit->header);
	unsigned long current = (unsigned long)slit + sizeof(acpi_slit_t);

	memset((void *)slit, 0, sizeof(acpi_slit_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "SLIT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_slit_t);
	header->revision = get_acpi_table_revision(SLIT);

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

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_hpet_t);
	header->revision = get_acpi_table_revision(HPET);

	/* Fill out HPET address. */
	addr->space_id = ACPI_ADDRESS_SPACE_MEMORY;
	addr->bit_width = 64;
	addr->bit_offset = 0;
	addr->addrl = CONFIG_HPET_ADDRESS & 0xffffffff;
	addr->addrh = ((unsigned long long)CONFIG_HPET_ADDRESS) >> 32;

	hpet->id = *(unsigned int *)CONFIG_HPET_ADDRESS;
	hpet->number = 0;
	hpet->min_tick = CONFIG_HPET_MIN_TICKS;

	header->checksum = acpi_checksum((void *)hpet, sizeof(acpi_hpet_t));
}

void acpi_create_vfct(const struct device *device,
		      acpi_vfct_t *vfct,
		      unsigned long (*acpi_fill_vfct)(const struct device *device,
		      acpi_vfct_t *vfct_struct, unsigned long current))
{
	acpi_header_t *header = &(vfct->header);
	unsigned long current = (unsigned long)vfct + sizeof(acpi_vfct_t);

	memset((void *)vfct, 0, sizeof(acpi_vfct_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "VFCT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->revision = get_acpi_table_revision(VFCT);

	current = acpi_fill_vfct(device, vfct, current);

	/* If no BIOS image, return with header->length == 0. */
	if (!vfct->VBIOSImageOffset)
		return;

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)vfct;
	header->checksum = acpi_checksum((void *)vfct, header->length);
}

void acpi_create_ipmi(const struct device *device,
		      struct acpi_spmi *spmi,
		      const u16 ipmi_revision,
		      const acpi_addr_t *addr,
		      const enum acpi_ipmi_interface_type type,
		      const s8 gpe_interrupt,
		      const u32 apic_interrupt,
		      const u32 uid)
{
	acpi_header_t *header = &(spmi->header);
	memset((void *)spmi, 0, sizeof(struct acpi_spmi));

	/* Fill out header fields. */
	memcpy(header->signature, "SPMI", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(struct acpi_spmi);
	header->revision = get_acpi_table_revision(SPMI);

	spmi->reserved = 1;

	if (device->path.type == DEVICE_PATH_PCI) {
		spmi->pci_device_flag = ACPI_IPMI_PCI_DEVICE_FLAG;
		spmi->pci_bus = device->bus->secondary;
		spmi->pci_device = device->path.pci.devfn >> 3;
		spmi->pci_function = device->path.pci.devfn & 0x7;
	} else if (type != IPMI_INTERFACE_SSIF) {
		memcpy(spmi->uid, &uid, sizeof(spmi->uid));
	}

	spmi->base_address = *addr;
	spmi->specification_revision = ipmi_revision;

	spmi->interface_type = type;

	if (gpe_interrupt >= 0 && gpe_interrupt < 32) {
		spmi->gpe = gpe_interrupt;
		spmi->interrupt_type |= ACPI_IPMI_INT_TYPE_SCI;
	}
	if (apic_interrupt > 0) {
		spmi->global_system_interrupt = apic_interrupt;
		spmi->interrupt_type |= ACPI_IPMI_INT_TYPE_APIC;
	}

	/* Calculate checksum. */
	header->checksum = acpi_checksum((void *)spmi, header->length);
}

void acpi_create_ivrs(acpi_ivrs_t *ivrs,
		      unsigned long (*acpi_fill_ivrs)(acpi_ivrs_t *ivrs_struct,
		      unsigned long current))
{
	acpi_header_t *header = &(ivrs->header);
	unsigned long current = (unsigned long)ivrs + sizeof(acpi_ivrs_t);

	memset((void *)ivrs, 0, sizeof(acpi_ivrs_t));

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "IVRS", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_ivrs_t);
	header->revision = get_acpi_table_revision(IVRS);

	current = acpi_fill_ivrs(ivrs, current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)ivrs;
	header->checksum = acpi_checksum((void *)ivrs, header->length);
}

unsigned long acpi_write_hpet(const struct device *device, unsigned long current,
	acpi_rsdp_t *rsdp)
{
	acpi_hpet_t *hpet;

	/*
	 * We explicitly add these tables later on:
	 */
	printk(BIOS_DEBUG, "ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	current = ALIGN_UP(current, 16);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	return current;
}

void acpi_create_dbg2(acpi_dbg2_header_t *dbg2,
		      int port_type, int port_subtype,
		      acpi_addr_t *address, uint32_t address_size,
		      const char *device_path)
{
	uintptr_t current;
	acpi_dbg2_device_t *device;
	uint32_t *dbg2_addr_size;
	acpi_header_t *header;
	size_t path_len;
	const char *path;
	char *namespace;

	/* Fill out header fields. */
	current = (uintptr_t)dbg2;
	memset(dbg2, 0, sizeof(acpi_dbg2_header_t));
	header = &(dbg2->header);

	if (!header)
		return;

	header->revision = get_acpi_table_revision(DBG2);
	memcpy(header->signature, "DBG2", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;

	/* One debug device defined */
	dbg2->devices_offset = sizeof(acpi_dbg2_header_t);
	dbg2->devices_count = 1;
	current += sizeof(acpi_dbg2_header_t);

	/* Device comes after the header */
	device = (acpi_dbg2_device_t *)current;
	memset(device, 0, sizeof(acpi_dbg2_device_t));
	current += sizeof(acpi_dbg2_device_t);

	device->revision = 0;
	device->address_count = 1;
	device->port_type = port_type;
	device->port_subtype = port_subtype;

	/* Base Address comes after device structure */
	memcpy((void *)current, address, sizeof(acpi_addr_t));
	device->base_address_offset = current - (uintptr_t)device;
	current += sizeof(acpi_addr_t);

	/* Address Size comes after address structure */
	dbg2_addr_size = (uint32_t *)current;
	device->address_size_offset = current - (uintptr_t)device;
	*dbg2_addr_size = address_size;
	current += sizeof(uint32_t);

	/* Namespace string comes last, use '.' if not provided */
	path = device_path ? : ".";
	/* Namespace string length includes NULL terminator */
	path_len = strlen(path) + 1;
	namespace = (char *)current;
	device->namespace_string_length = path_len;
	device->namespace_string_offset = current - (uintptr_t)device;
	strncpy(namespace, path, path_len);
	current += path_len;

	/* Update structure lengths and checksum */
	device->length = current - (uintptr_t)device;
	header->length = current - (uintptr_t)dbg2;
	header->checksum = acpi_checksum((uint8_t *)dbg2, header->length);
}

unsigned long acpi_write_dbg2_pci_uart(acpi_rsdp_t *rsdp, unsigned long current,
				const struct device *dev, uint8_t access_size)
{
	acpi_dbg2_header_t *dbg2 = (acpi_dbg2_header_t *)current;
	struct resource *res;
	acpi_addr_t address;

	if (!dev) {
		printk(BIOS_DEBUG, "%s: Device not found\n", __func__);
		return current;
	}
	if (!dev->enabled) {
		printk(BIOS_INFO, "%s: Device not enabled\n", __func__);
		return current;
	}
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "%s: Unable to find resource for %s\n",
		       __func__, dev_path(dev));
		return current;
	}

	memset(&address, 0, sizeof(address));
	if (res->flags & IORESOURCE_IO)
		address.space_id = ACPI_ADDRESS_SPACE_IO;
	else if (res->flags & IORESOURCE_MEM)
		address.space_id = ACPI_ADDRESS_SPACE_MEMORY;
	else {
		printk(BIOS_ERR, "%s: Unknown address space type\n", __func__);
		return current;
	}

	address.addrl = (uint32_t)res->base;
	address.addrh = (uint32_t)((res->base >> 32) & 0xffffffff);
	address.access_size = access_size;

	acpi_create_dbg2(dbg2,
			 ACPI_DBG2_PORT_SERIAL,
			 ACPI_DBG2_PORT_SERIAL_16550,
			 &address, res->size,
			 acpi_device_path(dev));

	if (dbg2->header.length) {
		current += dbg2->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, dbg2);
	}

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
	facs->version = get_acpi_table_revision(FACS);
}

static void acpi_write_rsdt(acpi_rsdt_t *rsdt, char *oem_id, char *oem_table_id)
{
	acpi_header_t *header = &(rsdt->header);

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "RSDT", 4);
	memcpy(header->oem_id, oem_id, 6);
	memcpy(header->oem_table_id, oem_table_id, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_rsdt_t);
	header->revision = get_acpi_table_revision(RSDT);

	/* Entries are filled in later, we come with an empty set. */

	/* Fix checksum. */
	header->checksum = acpi_checksum((void *)rsdt, sizeof(acpi_rsdt_t));
}

static void acpi_write_xsdt(acpi_xsdt_t *xsdt, char *oem_id, char *oem_table_id)
{
	acpi_header_t *header = &(xsdt->header);

	if (!header)
		return;

	/* Fill out header fields. */
	memcpy(header->signature, "XSDT", 4);
	memcpy(header->oem_id, oem_id, 6);
	memcpy(header->oem_table_id, oem_table_id, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->length = sizeof(acpi_xsdt_t);
	header->revision = get_acpi_table_revision(XSDT);

	/* Entries are filled in later, we come with an empty set. */

	/* Fix checksum. */
	header->checksum = acpi_checksum((void *)xsdt, sizeof(acpi_xsdt_t));
}

static void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt,
			    acpi_xsdt_t *xsdt, char *oem_id)
{
	memset(rsdp, 0, sizeof(acpi_rsdp_t));

	memcpy(rsdp->signature, RSDP_SIG, 8);
	memcpy(rsdp->oem_id, oem_id, 6);

	rsdp->length = sizeof(acpi_rsdp_t);
	rsdp->rsdt_address = (uintptr_t)rsdt;

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
		rsdp->xsdt_address = (u64)(uintptr_t)xsdt;
		rsdp->revision = get_acpi_table_revision(RSDP);
	}

	/* Calculate checksums. */
	rsdp->checksum = acpi_checksum((void *)rsdp, 20);
	rsdp->ext_checksum = acpi_checksum((void *)rsdp, sizeof(acpi_rsdp_t));
}

unsigned long acpi_create_hest_error_source(acpi_hest_t *hest,
	acpi_hest_esd_t *esd, u16 type, void *data, u16 data_len)
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
		hen->conf_we = 0;	/* Configuration Write Enable. */
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
	hest->error_source_count++;

	memcpy(pos, data, data_len);
	len += data_len;
	if (header)
		header->length += len;

	return len;
}

/* ACPI 4.0 */
void acpi_write_hest(acpi_hest_t *hest,
		     unsigned long (*acpi_fill_hest)(acpi_hest_t *hest))
{
	acpi_header_t *header = &(hest->header);

	memset(hest, 0, sizeof(acpi_hest_t));

	if (!header)
		return;

	memcpy(header->signature, "HEST", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;
	header->length += sizeof(acpi_hest_t);
	header->revision = get_acpi_table_revision(HEST);

	acpi_fill_hest(hest);

	/* Calculate checksums. */
	header->checksum = acpi_checksum((void *)hest, header->length);
}

/* ACPI 3.0b */
void acpi_write_bert(acpi_bert_t *bert, uintptr_t region, size_t length)
{
	acpi_header_t *header = &(bert->header);

	memset(bert, 0, sizeof(acpi_bert_t));

	if (!header)
		return;

	memcpy(header->signature, "BERT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;
	header->length += sizeof(acpi_bert_t);
	header->revision = get_acpi_table_revision(BERT);

	bert->error_region = region;
	bert->region_length = length;

	/* Calculate checksums. */
	header->checksum = acpi_checksum((void *)bert, header->length);
}

__weak void arch_fill_fadt(acpi_fadt_t *fadt) { }
__weak void soc_fill_fadt(acpi_fadt_t *fadt) { }
__weak void mainboard_fill_fadt(acpi_fadt_t *fadt) { }

void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	memset((void *) fadt, 0, sizeof(acpi_fadt_t));

	if (!header)
		return;

	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = get_acpi_table_revision(FADT);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;

	fadt->firmware_ctrl = (unsigned long) facs;
	fadt->x_firmware_ctl_l = (unsigned long)facs;
	fadt->x_firmware_ctl_h = 0;

	fadt->dsdt = (unsigned long) dsdt;
	fadt->x_dsdt_l = (unsigned long)dsdt;
	fadt->x_dsdt_h = 0;

	/* should be 0 ACPI 3.0 */
	fadt->reserved = 0;

	if (CONFIG(SYSTEM_TYPE_CONVERTIBLE) ||
	    CONFIG(SYSTEM_TYPE_LAPTOP))
		fadt->preferred_pm_profile = PM_MOBILE;
	else if (CONFIG(SYSTEM_TYPE_DETACHABLE) ||
		 CONFIG(SYSTEM_TYPE_TABLET))
		fadt->preferred_pm_profile = PM_TABLET;
	else
		fadt->preferred_pm_profile = PM_DESKTOP;

	arch_fill_fadt(fadt);

	acpi_fill_fadt(fadt);

	soc_fill_fadt(fadt);
	mainboard_fill_fadt(fadt);

	header->checksum =
	    acpi_checksum((void *) fadt, header->length);
}

unsigned long __weak fw_cfg_acpi_tables(unsigned long start)
{
	return 0;
}

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_xsdt_t *xsdt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *slic_file, *slic;
	acpi_header_t *ssdt;
	acpi_header_t *dsdt_file, *dsdt;
	acpi_mcfg_t *mcfg;
	acpi_tcpa_t *tcpa;
	acpi_tpm2_t *tpm2;
	acpi_madt_t *madt;
	struct device *dev;
	unsigned long fw;
	size_t slic_size, dsdt_size;
	char oem_id[6], oem_table_id[8];

	current = start;

	/* Align ACPI tables to 16byte */
	current = acpi_align_current(current);

	/* Special case for qemu */
	fw = fw_cfg_acpi_tables(current);
	if (fw) {
		rsdp = NULL;
		/* Find RSDP. */
		for (void *p = (void *)current; p < (void *)fw; p += 16) {
			if (valid_rsdp((acpi_rsdp_t *)p)) {
				rsdp = p;
				break;
			}
		}
		if (!rsdp)
			return fw;

		/* Add BOOT0000 for Linux google firmware driver */
		printk(BIOS_DEBUG, "ACPI:     * SSDT\n");
		ssdt = (acpi_header_t *)fw;
		current = (unsigned long)ssdt + sizeof(acpi_header_t);

		memset((void *)ssdt, 0, sizeof(acpi_header_t));

		memcpy(&ssdt->signature, "SSDT", 4);
		ssdt->revision = get_acpi_table_revision(SSDT);
		memcpy(&ssdt->oem_id, OEM_ID, 6);
		memcpy(&ssdt->oem_table_id, oem_table_id, 8);
		ssdt->oem_revision = 42;
		memcpy(&ssdt->asl_compiler_id, ASLC, 4);
		ssdt->asl_compiler_revision = asl_revision;
		ssdt->length = sizeof(acpi_header_t);

		acpigen_set_current((char *) current);

		/* Write object to declare coreboot tables */
		acpi_ssdt_write_cbtable();

		/* (Re)calculate length and checksum. */
		ssdt->length = current - (unsigned long)ssdt;
		ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);

		acpi_create_ssdt_generator(ssdt, ACPI_TABLE_CREATOR);

		acpi_add_table(rsdp, ssdt);

		return fw;
	}

	dsdt_file = cbfs_boot_map_with_leak(
				     CONFIG_CBFS_PREFIX "/dsdt.aml",
				     CBFS_TYPE_RAW, &dsdt_size);
	if (!dsdt_file) {
		printk(BIOS_ERR, "No DSDT file, skipping ACPI tables\n");
		return current;
	}

	if (dsdt_file->length > dsdt_size
	    || dsdt_file->length < sizeof(acpi_header_t)
	    || memcmp(dsdt_file->signature, "DSDT", 4) != 0) {
		printk(BIOS_ERR, "Invalid DSDT file, skipping ACPI tables\n");
		return current;
	}

	slic_file = cbfs_boot_map_with_leak(CONFIG_CBFS_PREFIX "/slic",
				     CBFS_TYPE_RAW, &slic_size);
	if (slic_file
	    && (slic_file->length > slic_size
		|| slic_file->length < sizeof(acpi_header_t)
		|| (memcmp(slic_file->signature, "SLIC", 4) != 0
		    && memcmp(slic_file->signature, "MSDM", 4) != 0))) {
		slic_file = 0;
	}

	if (slic_file) {
		memcpy(oem_id, slic_file->oem_id, 6);
		memcpy(oem_table_id, slic_file->oem_table_id, 8);
	} else {
		memcpy(oem_id, OEM_ID, 6);
		memcpy(oem_table_id, ACPI_TABLE_CREATOR, 8);
	}

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	current = acpi_align_current(current);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);
	current = acpi_align_current(current);
	xsdt = (acpi_xsdt_t *) current;
	current += sizeof(acpi_xsdt_t);
	current = acpi_align_current(current);

	/* clear all table memory */
	memset((void *) start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, xsdt, oem_id);
	acpi_write_rsdt(rsdt, oem_id, oem_table_id);
	acpi_write_xsdt(xsdt, oem_id, oem_table_id);

	printk(BIOS_DEBUG, "ACPI:    * FACS\n");
	current = ALIGN_UP(current, 64);
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	current = acpi_align_current(current);
	acpi_create_facs(facs);

	printk(BIOS_DEBUG, "ACPI:    * DSDT\n");
	dsdt = (acpi_header_t *) current;
	memcpy(dsdt, dsdt_file, sizeof(acpi_header_t));
	if (dsdt->length >= sizeof(acpi_header_t)) {
		current += sizeof(acpi_header_t);

		acpigen_set_current((char *) current);
		for (dev = all_devices; dev; dev = dev->next)
			if (dev->ops && dev->ops->acpi_inject_dsdt)
				dev->ops->acpi_inject_dsdt(dev);
		current = (unsigned long) acpigen_get_current();
		memcpy((char *)current,
		       (char *)dsdt_file + sizeof(acpi_header_t),
		       dsdt->length - sizeof(acpi_header_t));
		current += dsdt->length - sizeof(acpi_header_t);

		/* (Re)calculate length and checksum. */
		dsdt->length = current - (unsigned long)dsdt;
		dsdt->checksum = 0;
		dsdt->checksum = acpi_checksum((void *)dsdt, dsdt->length);
	}

	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "ACPI:    * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	current = acpi_align_current(current);

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

	if (slic_file) {
		printk(BIOS_DEBUG, "ACPI:     * SLIC\n");
		slic = (acpi_header_t *)current;
		memcpy(slic, slic_file, slic_file->length);
		current += slic_file->length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, slic);
	}

	printk(BIOS_DEBUG, "ACPI:     * SSDT\n");
	ssdt = (acpi_header_t *)current;
	acpi_create_ssdt_generator(ssdt, ACPI_TABLE_CREATOR);
	if (ssdt->length > sizeof(acpi_header_t)) {
		current += ssdt->length;
		acpi_add_table(rsdp, ssdt);
		current = acpi_align_current(current);
	}

	printk(BIOS_DEBUG, "ACPI:    * MCFG\n");
	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	if (mcfg->header.length > sizeof(acpi_mcfg_t)) {
		current += mcfg->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, mcfg);
	}

	if (CONFIG(TPM1)) {
		printk(BIOS_DEBUG, "ACPI:    * TCPA\n");
		tcpa = (acpi_tcpa_t *) current;
		acpi_create_tcpa(tcpa);
		if (tcpa->header.length >= sizeof(acpi_tcpa_t)) {
			current += tcpa->header.length;
			current = acpi_align_current(current);
			acpi_add_table(rsdp, tcpa);
		}
	}

	if (CONFIG(TPM2)) {
		printk(BIOS_DEBUG, "ACPI:    * TPM2\n");
		tpm2 = (acpi_tpm2_t *) current;
		acpi_create_tpm2(tpm2);
		if (tpm2->header.length >= sizeof(acpi_tpm2_t)) {
			current += tpm2->header.length;
			current = acpi_align_current(current);
			acpi_add_table(rsdp, tpm2);
		}
	}

	printk(BIOS_DEBUG, "ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	if (madt->header.length > sizeof(acpi_madt_t)) {
		current += madt->header.length;
		acpi_add_table(rsdp, madt);
	}
	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->write_acpi_tables) {
			current = dev->ops->write_acpi_tables(dev, current,
				rsdp);
			current = acpi_align_current(current);
		}
	}

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
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

void *acpi_find_wakeup_vector(void)
{
	char *p, *end;
	acpi_rsdt_t *rsdt;
	acpi_facs_t *facs;
	acpi_fadt_t *fadt = NULL;
	acpi_rsdp_t *rsdp = NULL;
	void *wake_vec;
	int i;

	if (!acpi_is_wakeup())
		return NULL;

	printk(BIOS_DEBUG, "Trying to find the wakeup vector...\n");

	/* Find RSDP. */
	for (p = (char *)0xe0000; p < (char *)0xfffff; p += 16) {
		rsdp = valid_rsdp((acpi_rsdp_t *)p);
		if (rsdp)
			break;
	}

	if (rsdp == NULL) {
		printk(BIOS_ALERT,
		       "No RSDP found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "RSDP found at %p\n", rsdp);
	rsdt = (acpi_rsdt_t *)(uintptr_t)rsdp->rsdt_address;

	end = (char *)rsdt + rsdt->header.length;
	printk(BIOS_DEBUG, "RSDT found at %p ends at %p\n", rsdt, end);

	for (i = 0; ((char *)&rsdt->entry[i]) < end; i++) {
		fadt = (acpi_fadt_t *)(uintptr_t)rsdt->entry[i];
		if (strncmp((char *)fadt, "FACP", 4) == 0)
			break;
		fadt = NULL;
	}

	if (fadt == NULL) {
		printk(BIOS_ALERT,
		       "No FADT found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "FADT found at %p\n", fadt);
	facs = (acpi_facs_t *)(uintptr_t)fadt->firmware_ctrl;

	if (facs == NULL) {
		printk(BIOS_ALERT,
		       "No FACS found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "FACS found at %p\n", facs);
	wake_vec = (void *)(uintptr_t)facs->firmware_waking_vector;
	printk(BIOS_DEBUG, "OS waking vector is %p\n", wake_vec);

	return wake_vec;
}

__weak int acpi_get_gpe(int gpe)
{
	return -1; /* implemented by SOC */
}

int get_acpi_table_revision(enum acpi_tables table)
{
	switch (table) {
	case FADT:
		return ACPI_FADT_REV_ACPI_6_0;
	case MADT: /* ACPI 3.0: 2, ACPI 4.0/5.0: 3, ACPI 6.2b/6.3: 5 */
		return 3;
	case MCFG:
		return 1;
	case TCPA:
		return 2;
	case TPM2:
		return 4;
	case SSDT: /* ACPI 3.0 upto 6.3: 2 */
		return 2;
	case SRAT: /* ACPI 2.0: 1, ACPI 3.0: 2, ACPI 4.0 upto 6.3: 3 */
		return 1; /* TODO Should probably be upgraded to 2 */
	case DMAR:
		return 1;
	case SLIT: /* ACPI 2.0 upto 6.3: 1 */
		return 1;
	case SPMI: /* IMPI 2.0 */
		return 5;
	case HPET: /* Currently 1. Table added in ACPI 2.0. */
		return 1;
	case VFCT: /* ACPI 2.0/3.0/4.0: 1 */
		return 1;
	case IVRS:
		return IVRS_FORMAT_MIXED;
	case DBG2:
		return 0;
	case FACS: /* ACPI 2.0/3.0: 1, ACPI 4.0 upto 6.3: 2 */
		return 1;
	case RSDT: /* ACPI 1.0 upto 6.3: 1 */
		return 1;
	case XSDT: /* ACPI 2.0 upto 6.3: 1 */
		return 1;
	case RSDP: /* ACPI 2.0 upto 6.3: 2 */
		return 2;
	case HEST:
		return 1;
	case NHLT:
		return 5;
	case BERT:
		return 1;
	default:
		return -1;
	}
	return -1;
}
