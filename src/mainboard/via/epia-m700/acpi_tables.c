/*
 * This file is part of the coreboot project.
 *
 * LinuxBIOS ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 * ACPI FADT, FACS, and DSDT table support added by
 * Nick Barker <nick.barker9@btinternet.com>, and those portions
 * (C) Copyright 2004 Nick Barker
 * (C) Copyright 2005 Stefan Reinauer
 * (C) Copyright 2009 One Laptop per Child, Association, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * Most parts of this file copied from asus\a8v-e_se\acpi_tables.c,
 * acpi_is_wakeup() is from Rudolf's S3 patch and SSDT was added.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <../../../northbridge/via/vx800/vx800.h>

extern unsigned char AmlCode_dsdt[];
extern unsigned char AmlCode_ssdt[];

extern u32 wake_vec;
extern u8 acpi_sleep_type;

/*
 * These four macros are copied from <arch/smp/mpspec.h>, I have to do this
 * since the "default HAVE_MP_TABLE = 0" in Options.lb, and also since
 * mainboard/via/... have no mptable.c (so that I can not set
 * HAVE_MP_TABLE = 1) as many other mainboards.
 * So I have to copy these four to here. acpi_fill_madt() needs this.
 */
#define MP_IRQ_POLARITY_HIGH	0x1
#define MP_IRQ_POLARITY_LOW	0x3
#define MP_IRQ_TRIGGER_EDGE	0x4
#define MP_IRQ_TRIGGER_LEVEL	0xc

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* NO MCFG in VX855, no PCI-E. */
	return current;
}

unsigned long acpi_create_madt_lapics(unsigned long current)
{
	device_t cpu;
	int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_APIC_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current,
					   cpu_index, cpu->path.apic.apic_id);
		cpu_index++;
	}
	return current;
}

unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags,
					  u8 lint)
{
	device_t cpu;
	int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_APIC_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
					      current, cpu_index, flags, lint);
		cpu_index++;
	}
	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base = 0x18;

	/* Create all subtables for processors. */
	current = acpi_create_madt_lapics(current);

	/* Write SB IOAPIC. */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				   VX800SB_APIC_ID, VX800SB_APIC_BASE, 0);

	/* IRQ0 -> APIC IRQ2. */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0x0);

	/* IRQ9 ACPI active low. */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	/* Create all subtables for processors. */
	current = acpi_create_madt_lapic_nmis(current,
			      MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 1);

	return current;
}

unsigned long acpi_fill_slit(unsigned long current)
{
	/* Not implemented. */
	return current;
}

unsigned long acpi_fill_srat(unsigned long current)
{
	/* No NUMA, no SRAT. */
}

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_srat_t *srat;
	acpi_rsdt_t *rsdt;
	acpi_mcfg_t *mcfg;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *dsdt, *ssdt;

	/* Align ACPI tables to 16 byte. */
	start = (start + 0x0f) & -0x10;
	current = start;

	printk_info("ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT table. */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* Clear all table memory. */
	memset((void *)start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt);
	acpi_write_rsdt(rsdt);

	/* We explicitly add these tables later on: */
	printk_debug("ACPI:     * FACS\n");
	current = ALIGN(current, 64);
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	printk_debug("ACPI:     * DSDT\n");
	dsdt = (acpi_header_t *) current;
	current += ((acpi_header_t *) AmlCode_dsdt)->length;
	memcpy((void *)dsdt, (void *)AmlCode_dsdt,
	       ((acpi_header_t *) AmlCode_dsdt)->length);
	dsdt->checksum = 0; /* Don't trust iasl to get this right. */
	dsdt->checksum = acpi_checksum(dsdt, dsdt->length);
	printk_debug("ACPI:     * DSDT @ %08x Length %x\n", dsdt, dsdt->length);

	printk_debug("ACPI:     * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdt, fadt);

	/* If we want to use HPET timers Linux wants it in MADT. */
	printk_debug("ACPI:    * MADT\n");
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	acpi_add_table(rsdt, madt);

	/* NO MCFG in VX855, no PCI-E. */

	printk_debug("ACPI:    * HPET\n");
	hpet = (acpi_mcfg_t *) current;
	acpi_create_hpet(hpet);
	current += hpet->header.length;
	acpi_add_table(rsdt, hpet);

#if 0
	printk_debug("ACPI:     * SSDT\n");
	ssdt = (acpi_header_t *) current;
	current += ((acpi_header_t *)AmlCode_ssdt)->length;
	memcpy((void *)ssdt,(void *)AmlCode_ssdt, ((acpi_header_t *)AmlCode_ssdt)->length);
	ssdt->checksum = 0; /* Don't trust iasl to get this right. */
	ssdt->checksum = acpi_checksum(ssdt, ssdt->length);
	acpi_add_table(rsdt, ssdt);
	printk_debug("ACPI:     * SSDT @ %08x Length %x\n", ssdt, ssdt->length);
#endif

	printk_info("ACPI: done.\n");
	return current;
}

int acpi_is_wakeup(void)
{
	return (acpi_sleep_type == 3);
}
