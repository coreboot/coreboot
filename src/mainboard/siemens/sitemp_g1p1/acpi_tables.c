/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include <../../../northbridge/amd/amdk8/acpi.h>
#include <arch/cpu.h>
#include <cpu/amd/model_fxx_powernow.h>
#include <southbridge/amd/rs690/rs690.h>

#define DUMP_ACPI_TABLES 0

extern u16 pm_base;

/*
* Assume the max pstate number is 8
* 0x21(33 bytes) is one package length of _PSS package
*/

#define Maxpstate 8
#define Defpkglength 0x21
#define GLOBAL_VARS_SIZE 0x100

typedef struct {
	/* Miscellaneous */
	u16 osys;
	u16 linx;
	u32	pcba;
	u8  mpen;
	u8 reserv[247];
} __attribute__((packed)) global_vars_t;

static void acpi_write_gvars(global_vars_t *gvars)
{
	device_t dev;
	struct resource *res;

	memset((void *)gvars, 0, GLOBAL_VARS_SIZE);

	gvars->pcba = EXT_CONF_BASE_ADDRESS;
	dev = dev_find_slot(0, PCI_DEVFN(0,0));
	res = probe_resource(dev, 0x1C);
	if( res )
		gvars->pcba = res->base;

	gvars->mpen = 1;
}

#if DUMP_ACPI_TABLES == 1
static void dump_mem(u32 start, u32 end)
{
	u32 i;
	print_debug("dump_mem:");
	for (i = start; i < end; i++) {
		if ((i & 0xf) == 0) {
			printk(BIOS_DEBUG, "\n%08x:", i);
		}
		printk(BIOS_DEBUG, " %02x", (u8)*((u8 *)i));
	}
	print_debug("\n");
}
#endif

extern const unsigned char AmlCode[];

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct resource *res;
	resource_t mmconf_base = EXT_CONF_BASE_ADDRESS; // default

	device_t dev = dev_find_slot(0,PCI_DEVFN(0,0));
	// we report mmconf base
	res = probe_resource(dev, 0x1C);
	if( res )
		mmconf_base = res->base;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current, mmconf_base, 0x0, 0x0, 0x1f); // Fix me: should i reserve 255 busses ?

	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write SB600 IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2,
					   IO_APIC_ADDR, 0);
#if !CONFIG_LINT01_CONVERSION
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0);

	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);
#else
						/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edige-triggered, Active high */

	/* create all subtables for processors */
	current = acpi_create_madt_lapic_nmis(current, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);
	/* 1: LINT1 connect to NMI */
	set_nbcfg_enable_bits(dev_find_slot(0, PCI_DEVFN(0x18, 0)), 0x68, 1 << 16, 1 << 16); // Local Interrupt Conversion Enable
#endif
	return current;
}

unsigned long acpi_fill_ssdt_generator(unsigned long current, const char *oem_table_id) {
	k8acpi_write_vars();
	amd_model_fxx_generate_powernow(pm_base + 8, 6, 1);
	return (unsigned long) (acpigen_get_current());
}

#define ALIGN_CURRENT current = (ALIGN(current, 16))

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	int i;

	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_srat_t *srat;
	acpi_xsdt_t *xsdt;
	acpi_mcfg_t *mcfg;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *dsdt;
	acpi_header_t *ssdt;

	get_bus_conf();		/* it will get sblk, pci1234, hcdn, and sbdn */

	/* Align ACPI tables to 16byte */
	current = start;
	ALIGN_CURRENT;

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx...\n", start);

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
	memset((void *)start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, xsdt);
	acpi_write_rsdt(rsdt);
	acpi_write_xsdt(xsdt);
	/*
	 * We explicitly add these tables later on:
	 */
	current = ALIGN(current, 64);
	/* FACS */
	printk(BIOS_DEBUG, "ACPI:     * FACS\n");
	facs = (acpi_facs_t *) current;
	acpi_create_facs(facs);
	current += sizeof(acpi_facs_t);

	 /* HPET */
	printk(BIOS_DEBUG, "ACPI:    * HPET\n");
	hpet = (acpi_hpet_t *) current;
	acpi_create_hpet(hpet);
	current += sizeof(acpi_hpet_t);
	acpi_add_table(rsdp, hpet);

	/* If we want to use HPET Timers Linux wants an MADT */
	printk(BIOS_DEBUG, "ACPI:    * MADT\n");
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	acpi_add_table(rsdp, madt);

	/* MCFG */
	printk(BIOS_DEBUG, "ACPI:    * MCFG\n");
	mcfg = (acpi_mcfg_t *) current;
	acpi_create_mcfg(mcfg);
	current += mcfg->header.length;
	acpi_add_table(rsdp, mcfg);

	/* SSDT */
	printk(BIOS_DEBUG, "ACPI:    * SSDT\n");
	ssdt = (acpi_header_t *)current;
	acpi_create_ssdt_generator(ssdt, ACPI_TABLE_CREATOR);
	current += ssdt->length;
	acpi_add_table(rsdp, ssdt);

	/* DSDT */
	printk(BIOS_DEBUG, "ACPI:    * DSDT\n");
	dsdt = (acpi_header_t *)current;
	memcpy(dsdt, &AmlCode, sizeof(acpi_header_t));
	current += dsdt->length;
	memcpy(dsdt, &AmlCode, dsdt->length);

	/* Pack gvars into the ACPI table area */
	for (i=0; i < dsdt->length; i++) {
		if (*(u32*)(((u32)dsdt) + i) == 0xBADEAFFE) {
			printk(BIOS_DEBUG, "ACPI: Patching up globals in DSDT at offset 0x%04x -> 0x%08lx\n", i, current);
			*(u32*)(((u32)dsdt) + i) = current;
			break;
		}
	}

	/* And fill it */
	acpi_write_gvars((global_vars_t *)current);
	current += GLOBAL_VARS_SIZE;
	/* We patched up the DSDT, so we need to recalculate the checksum */
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum((void *)dsdt, dsdt->length);
	printk(BIOS_DEBUG, "ACPI:    * DSDT @ %p Length %x\n", dsdt, dsdt->length);

	/* FADT */
	printk(BIOS_DEBUG, "ACPI:    * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);
	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdp, fadt);

	/* SRAT */
	printk(BIOS_DEBUG, "ACPI:    * SRAT\n");
	srat = (acpi_srat_t *) current;
	acpi_create_srat(srat);
	acpi_add_table(rsdp, srat);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	printk(BIOS_INFO, "ACPI: done.\n");
	return current;
}
