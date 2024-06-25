/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/ioapic.h>
#include <cpu/cpu.h>
#include <version.h>

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

	dmar->host_address_width = soc_phys_address_size() - 1;
	dmar->flags = flags;

	current = acpi_fill_dmar(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)dmar;
	header->checksum = acpi_checksum((void *)dmar, header->length);
}

unsigned long acpi_create_dmar_drhd_4k(unsigned long current, u8 flags,
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

unsigned long acpi_create_dmar_satc(unsigned long current, u8 flags, u16 segment)
{
	dmar_satc_entry_t *satc = (dmar_satc_entry_t *)current;
	int satc_len = sizeof(dmar_satc_entry_t);
	memset(satc, 0, satc_len);
	satc->type = DMAR_SATC;
	satc->length = satc_len;
	satc->flags = flags;
	satc->segment_number = segment;

	return satc->length;
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

void acpi_dmar_satc_fixup(unsigned long base, unsigned long current)
{
	dmar_satc_entry_t *satc = (dmar_satc_entry_t *)base;
	satc->length = current - base;
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

unsigned long acpi_create_dmar_ds_ioapic_from_hw(unsigned long current,
						 u32 addr, u8 bus, u8 dev, u8 fn)
{
	u8 enumeration_id = get_ioapic_id((uintptr_t)addr);
	return acpi_create_dmar_ds(current,
			SCOPE_IOAPIC, enumeration_id, bus, dev, fn);
}

unsigned long acpi_create_dmar_ds_msi_hpet(unsigned long current,
	u8 enumeration_id, u8 bus, u8 dev, u8 fn)
{
	return acpi_create_dmar_ds(current,
			SCOPE_MSI_HPET, enumeration_id, bus, dev, fn);
}
