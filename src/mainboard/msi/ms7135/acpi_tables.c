/*
 * ACPI support
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
#include <arch/io.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "../../../northbridge/amd/amdk8/acpi.h"
//#include <cpu/amd/model_fxx_powernow.h>

extern const unsigned char AmlCode[];

static void acpi_create_hpet_new(acpi_hpet_t *);
static int acpi_create_hpet_new_fill(acpi_hpet_t *, u32, u16, u8);
static unsigned long acpi_fill_hpet_new(unsigned long);

void acpi_create_hpet_new(acpi_hpet_t *hpet)
{
	acpi_header_t *header=&(hpet->header);
	unsigned long current=(unsigned long)hpet;

	memset((void *)hpet, 0, sizeof(acpi_hpet_t));

	/* fill out header fields */
	memcpy(header->signature, "HPET", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = sizeof(acpi_hpet_t);
	header->revision = 1;

	current = acpi_fill_hpet_new(current);

	/* recalculate length */
	header->length = current - (unsigned long)hpet;

	header->checksum	= acpi_checksum((void *)hpet, header->length);
}


int acpi_create_hpet_new_fill(acpi_hpet_t *hpet, u32 base, u16 min, u8 attr)
{
	static u8 num = 0;
	acpi_addr_t *addr = &(hpet->addr);

	hpet->id = read32(base + 0x000);

	/* fill out HPET address */
	addr->space_id		= 0; /* Memory */
	addr->bit_width		= 0;
	addr->bit_offset	= 0;
	addr->addrl		= base;
	addr->addrh		= 0;

	hpet->number	= num++;
	hpet->min_tick  = min;
	hpet->attributes = attr;

	return (sizeof(acpi_hpet_t));
}

static unsigned long acpi_fill_hpet_new(unsigned long current)
{
#if 1
	device_t dev;
	unsigned long hpet_base;

	dev = dev_find_slot(0x0, PCI_DEVFN(0x1,0));
	if (!dev)
		return current;

	hpet_base = pci_read_config32(dev, 0x44) & ~0xf;

	printk(BIOS_INFO, "hpet_base %lx.\n", hpet_base);

	current += acpi_create_hpet_new_fill((acpi_hpet_t *)current, hpet_base, 250, 1);
#endif

	return current;
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	device_t dev;
	unsigned long mcfg_base;

	dev = dev_find_slot(0x0, PCI_DEVFN(0x0,0));
	if (!dev)
		return current;

	mcfg_base = pci_read_config16(dev, 0x90);
	if ((mcfg_base & 0x1000) == 0)
		return current;

	mcfg_base = (mcfg_base & 0xf) << 28;

	printk(BIOS_INFO, "mcfg_base %lx.\n", mcfg_base);

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)
			current, mcfg_base, 0x0, 0x0, 0xff);
	return current;
}

/* APIC */
unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned long apic_addr;
	device_t dev;

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write NVIDIA CK804 IOAPIC. */
	dev = dev_find_slot(0x0, PCI_DEVFN(0x1,0));
	if (dev) {
		apic_addr = pci_read_config32(dev, PCI_BASE_ADDRESS_1) & ~0xf;
		current += acpi_create_madt_ioapic(
			(acpi_madt_ioapic_t *)current,
			CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS,
			apic_addr, 0);
		/* Initialize interrupt mapping if mptable.c didn't. */
#if (!CONFIG_GENERATE_MP_TABLE)
#error untested config
		{
			u32 dword;
			dword = 0x0120d218;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x12008a00;
			pci_write_config32(dev, 0x80, dword);

			dword = 0x0000007d;
			pci_write_config32(dev, 0x84, dword);
		}
#endif
	}

	/* IRQ of timer */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 0, 2, 0);
	/* IRQ9 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	/* create all subtables for processors */
	/* acpi_create_madt_lapic_nmis returns current, not size. */
	current = acpi_create_madt_lapic_nmis(current,
			MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);

	return current;
}

unsigned long acpi_fill_ssdt_generator(unsigned long current, const char *oem_table_id) {
	k8acpi_write_vars();
	//amd_model_fxx_generate_powernow(0, 0, 0);
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
	acpi_mcfg_t *mcfg;
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
	mcfg = (acpi_mcfg_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * MCFG @ %p\n", mcfg);
	acpi_create_mcfg(mcfg);
	current += mcfg->header.length;
	acpi_add_table(rsdp, mcfg);

	current = ALIGN(current, 16);
	hpet = (acpi_hpet_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * HPET @ %p\n", hpet);
	acpi_create_hpet_new(hpet);
	acpi_add_table(rsdp, hpet);
	current += hpet->header.length;

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
