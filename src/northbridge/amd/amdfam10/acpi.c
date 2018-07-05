/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdfam10_sysconf.h>
#include "amdfam10.h"

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
	if (res->index > 0xf)	/* Exclude MMIO resources, e.g. as set in northbridge.c amdfam10_domain_read_resources() */
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
	/* Implement SLIT algorithm in BKDG Rev. 3.62 Section 2.3.6.1
	 * Fill the first 8 bytes with the node number,
	 * then fill the next num*num byte with the distance,
	 * Distance entries vary with topology; the local node
	 * is always 10.
	 *
	 * Fully connected:
	 * Set all non-local nodes to 16
	 *
	 * Partially connected; with probe filter:
	 * Set all non-local nodes to 10+(num_hops*6)
	 *
	 * Partially connected; without probe filter:
	 * Set all non-local nodes to 13
	 *
	 * FIXME
	 * The partially connected cases are not implemented;
	 * once a means is found to detect partially connected
	 * topologies, implement the remaining cases.
	 */

	u8 *p = (u8 *)current;
	int nodes = sysconf.nodes;
	int i,j;

	memset(p, 0, 8+nodes*nodes);
	*p = (u8) nodes;
	p += 8;

	for (i = 0; i < nodes; i++) {
		for (j = 0; j < nodes; j++) {
			if (i == j)
				p[i*nodes+j] = 10;
			else
				p[i*nodes+j] = 16;
		}
	}

	current += 8+nodes*nodes;
	return current;
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

void northbridge_acpi_write_vars(struct device *device)
{
	/*
	 * If more than one physical CPU is installed, northbridge_acpi_write_vars()
	 * is called more than once and the resultant SSDT table is corrupted
	 * (duplicated entries).
	 * This prevents Linux from booting, with log messages like these:
	 * ACPI Error: [BUSN] Namespace lookup failure, AE_ALREADY_EXISTS (/dswload-353)
	 * ACPI Exception: AE_ALREADY_EXISTS, During name lookup/catalog (/psobject-222)
	 * followed by a slew of ACPI method failures and a hang when the invalid PCI
	 * resource entries are used.
	 * This routine prevents the SSDT table from being corrupted.
	 */
	static uint8_t ssdt_generated = 0;
	if (ssdt_generated)
		return;
	ssdt_generated = 1;

	msr_t msr;
	char pscope[] = "\\_SB.PCI0";
	int i;

	get_bus_conf();	/* it will get sblk, pci1234, hcdn, and sbdn */

	acpigen_write_scope(pscope);

	acpigen_write_name("BUSN");
	acpigen_write_package(HC_NUMS);
	for (i = 0; i < HC_NUMS; i++) {
		acpigen_write_dword(sysconf.ht_c_conf_bus[i]);
	}
	// minus the opcode
	acpigen_pop_len();

	acpigen_write_name("MMIO");

	acpigen_write_package(HC_NUMS * 4);

	for (i = 0; i<(HC_NUMS*2); i++) { // FIXME: change to more chain
		acpigen_write_dword(sysconf.conf_mmio_addrx[i]); //base
		acpigen_write_dword(sysconf.conf_mmio_addr[i]); //mask
	}
	// minus the opcode
	acpigen_pop_len();

	acpigen_write_name("PCIO");

	acpigen_write_package(HC_NUMS * 2);

	for (i = 0; i < HC_NUMS; i++) { // FIXME: change to more chain
		acpigen_write_dword(sysconf.conf_io_addrx[i]);
		acpigen_write_dword(sysconf.conf_io_addr[i]);
	}

	// minus the opcode
	acpigen_pop_len();

	acpigen_write_name_byte("SBLK", sysconf.sblk);

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


	acpigen_write_name_dword("SBDN", sysconf.sbdn);

	acpigen_write_name("HCLK");

	acpigen_write_package(HC_POSSIBLE_NUM);

	for (i = 0; i < sysconf.hc_possible_num; i++) {
		acpigen_write_dword(sysconf.pci1234[i]);
	}
	for (i = sysconf.hc_possible_num; i < HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		acpigen_write_dword(0x00000000);
	}
	// minus the opcode
	acpigen_pop_len();

	acpigen_write_name("HCDN");

	acpigen_write_package(HC_POSSIBLE_NUM);

	for (i = 0; i < sysconf.hc_possible_num; i++) {
		acpigen_write_dword(sysconf.hcdn[i]);
	}
	for (i = sysconf.hc_possible_num; i < HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		acpigen_write_dword(0x20202020);
	}
	// minus the opcode
	acpigen_pop_len();

	acpigen_write_name_byte("CBB", CONFIG_CBB);

	u8 CBST, CBB2, CBS2;

	if (CONFIG_CBB == 0xff) {
		CBST = (u8) (0x0f);
	} else {
		if ((sysconf.pci1234[0] >> 12) & 0xff) { //sb chain on  other than bus 0
			CBST = (u8) (0x0f);
		} else {
			CBST = (u8) (0x00);
		}
	}

	acpigen_write_name_byte("CBST", CBST);

	if ((CONFIG_CBB == 0xff) && (sysconf.nodes > 32)) {
		 CBS2 = 0x0f;
		 CBB2 = (u8)(CONFIG_CBB-1);
	} else {
		CBS2 = 0x00;
		CBB2 = 0x00;
	}

	acpigen_write_name_byte("CBB2", CBB2);
	acpigen_write_name_byte("CBS2", CBS2);

	//minus opcode
	acpigen_pop_len();
}

unsigned long northbridge_write_acpi_tables(struct device *device,
					    unsigned long current,
					    struct acpi_rsdp *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;

	get_bus_conf();	/* it will get sblk, pci1234, hcdn, and sbdn */

	/* SRAT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *) current;
	acpi_create_srat(srat, acpi_fill_srat);
	current += srat->header.length;
	acpi_add_table(rsdp, srat);

	/* SLIT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *) current;
	acpi_create_slit(slit, acpi_fill_slit);
	current += slit->header.length;
	acpi_add_table(rsdp, slit);

	return current;
}
