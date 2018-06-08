/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Description: Add madt lapic creat dynamically and SRAT related by yhlu
*/

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "acpi.h"

//it seems some functions can be moved arch/x86/boot/acpi.c

unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags, u8 lint)
{
	struct device *cpu;
	int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, cpu_index, flags, lint);
		cpu_index++;
	}
	return current;
}

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	struct device *cpu;
	int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		    (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		printk(BIOS_DEBUG, "SRAT: lapic cpu_index=%02x, node_id=%02x, apic_id=%02x\n", cpu_index, cpu->path.apic.node_id, cpu->path.apic.apic_id);
		current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current, cpu->path.apic.node_id, cpu->path.apic.apic_id);
		cpu_index++;
	}
	return current;
}

static unsigned long resk(uint64_t value)
{
	unsigned long resultk;
	if (value < (1ULL << 42)) {
		resultk = value >> 10;
	} else {
		resultk = 0xffffffff;
	}
	return resultk;
}

struct acpi_srat_mem_state {
	unsigned long current;
};

static void set_srat_mem(void *gp, struct device *dev, struct resource *res)
{
	struct acpi_srat_mem_state *state = gp;
	unsigned long basek, sizek;
	basek = resk(res->base);
	sizek = resk(res->size);

	printk(BIOS_DEBUG, "set_srat_mem: dev %s, res->index=%04lx startk=%08lx, sizek=%08lx\n",
		     dev_path(dev), res->index, basek, sizek);
	/*
	 * 0-640K must be on node 0
	 * next range is from 1M---
	 * So will cut off before 1M in the mem range
	 */
	if ((basek+sizek)<1024) return;

	if (basek < 1024) {
		sizek -= 1024 - basek;
		basek = 1024;
	}

	// need to figure out NV
	state->current += acpi_create_srat_mem((acpi_srat_mem_t *)state->current, (res->index & 0xf), basek, sizek, 1);
}

static unsigned long acpi_fill_srat(unsigned long current)
{
	struct acpi_srat_mem_state srat_mem_state;

	/* create all subtables for processors */
	current = acpi_create_srat_lapics(current);

	/* create all subteble for memory range */

	/* 0-640K must be on node 0 */
	current += acpi_create_srat_mem((acpi_srat_mem_t *)current, 0, 0, 640, 1);//enable

	srat_mem_state.current = current;
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_CACHEABLE, IORESOURCE_MEM | IORESOURCE_CACHEABLE,
		set_srat_mem, &srat_mem_state);

	current = srat_mem_state.current;
	return current;
}

static unsigned long acpi_fill_slit(unsigned long current)
{
	/* need to find out the node num at first */
	/* fill the first 8 byte with that num */
	/* fill the next num*num byte with distance, local is 10, 1 hop mean 20, and 2 hop with 30.... */

	/* because We has assume that we know the topology of the HT connection, So we can have set if we know the node_num */
	static u8 hops_8[] = {   0, 1, 1, 2, 2, 3, 3, 4,
				      1, 0, 2, 1, 3, 2, 4, 3,
				      1, 2, 0, 1, 1, 2, 2, 3,
				      2, 1, 1, 0, 2, 1, 3, 2,
				      2, 3, 1, 2, 0, 1, 1, 2,
				      3, 2, 2, 1, 1, 0, 2, 1,
				      3, 4, 2, 3, 1, 2, 0, 1,
				      4, 4, 3, 2, 2, 1, 1, 0 };

	u8 *p = (u8 *)current;
	int nodes = sysconf.nodes;
	int i,j;
	memset(p, 0, 8+nodes*nodes);
	*p = (u8) nodes;
	p += 8;

	for (i = 0; i < nodes; i++) {
		for (j = 0; j < nodes; j++) {
			if (i == j) {
				p[i*nodes+j] = 10;
			} else {
				p[i*nodes+j] = hops_8[i*nodes+j] * 2 + 10;

			}
		}
	}

	current += 8+nodes*nodes;

	return current;
}

unsigned long northbridge_write_acpi_tables(
		struct device *device,
		unsigned long start,
		acpi_rsdp_t *rsdp)
{
	unsigned long current;
	acpi_srat_t *srat;
	acpi_slit_t *slit;

	current = start;

	/* Fills sysconf structure needed for SRAT and SLIT.  */
	get_bus_conf();

	current = ALIGN(current, 16);
	srat = (acpi_srat_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * SRAT @ %p\n", srat);
	acpi_create_srat(srat, acpi_fill_srat);
	current += srat->header.length;
	acpi_add_table(rsdp, srat);

	/* SLIT */
	current = ALIGN(current, 16);
	slit = (acpi_slit_t *) current;
	printk(BIOS_DEBUG, "ACPI:    * SLIT @ %p\n", slit);
	acpi_create_slit(slit, acpi_fill_slit);
	current+=slit->header.length;
	acpi_add_table(rsdp,slit);

	return current;
}

static void k8acpi_write_HT(void) {
	int i;

	acpigen_write_name("HCLK");
	acpigen_write_package(HC_POSSIBLE_NUM);

	for (i = 0; i < sysconf.hc_possible_num; i++) {
		acpigen_write_dword(sysconf.pci1234[i]);
	}
	for (i = sysconf.hc_possible_num; i < HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		acpigen_write_dword(0x0);
	}

	acpigen_pop_len();

	acpigen_write_name("HCDN");
	acpigen_write_package(HC_POSSIBLE_NUM);

	for (i = 0; i < sysconf.hc_possible_num; i++) {
		acpigen_write_dword(sysconf.hcdn[i]);
	}
	for (i = sysconf.hc_possible_num; i < HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		acpigen_write_dword(0x20202020);
	}
	acpigen_pop_len();
}

static void k8acpi_write_pci_data(int dlen, const char *name, int offset) {
	struct device *dev;
	uint32_t dword;
	int i;

	dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));

	acpigen_write_name(name);
	acpigen_write_package(dlen);
	for (i = 0; i < dlen; i++) {
		dword = pci_read_config32(dev, offset+i*4);
		acpigen_write_dword(dword);
	}
	// minus the opcode
	acpigen_pop_len();
}

void k8acpi_write_vars(struct device *device)
{
	/*
	 * If more than one physical CPU is installed k8acpi_write_vars()
	 * is called more than once. If we don't prevent it, a SSDT table
	 * with duplicate variables will cause some ACPI parsers to be
	 * confused enough to fail.
	 */
	static uint8_t ssdt_generated = 0;
	if (ssdt_generated)
		return;
	ssdt_generated = 1;

	msr_t msr;
	char pscope[] = "\\_SB.PCI0";

	acpigen_write_scope(pscope);
	k8acpi_write_pci_data(4, "BUSN", 0xe0);
	k8acpi_write_pci_data(8, "PCIO", 0xc0);
	k8acpi_write_pci_data(16, "MMIO", 0x80);
	acpigen_write_name_byte("SBLK", sysconf.sblk);
	acpigen_write_name_byte("CBST",
	    ((sysconf.pci1234[0] >> 12) & 0xff) ? 0xf : 0x0);
	acpigen_write_name_dword("SBDN", sysconf.sbdn);
	msr = rdmsr(TOP_MEM);
	acpigen_write_name_dword("TOM1", msr.lo);
	msr = rdmsr(TOP_MEM2);
	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", (msr.hi << 12) | msr.lo >> 20);

	k8acpi_write_HT();
	//minus opcode
	acpigen_pop_len();
}

void update_ssdtx(void *ssdtx, int i)
{
	u8 *PCI;
	u8 *HCIN;
	u8 *UID;

	PCI = ssdtx + 0x32;
	HCIN = ssdtx + 0x39;
	UID = ssdtx + 0x40;

	if (i < 7) {
		*PCI = (u8) ('4' + i - 1);
	} else {
		*PCI = (u8) ('A' + i - 1 - 6);
	}
	*HCIN = (u8) i;
	*UID = (u8) (i + 3);

	/* FIXME: need to update the GSI id in the ssdtx too */

}
