/*
 * This file is part of the coreboot project.
 *
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdfam10_sysconf.h>
#include "amdfam10.h"

//it seems some functions can be moved arch/x86/boot/acpi.c

unsigned long acpi_create_madt_lapic_nmis(unsigned long current, u16 flags, u8 lint)
{
	device_t cpu;
	int cpu_index = 0;

	for(cpu = all_devices; cpu; cpu = cpu->next) {
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
	device_t cpu;
	int cpu_index = 0;

	for(cpu = all_devices; cpu; cpu = cpu->next) {
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
	if((basek+sizek)<1024) return;

	if(basek<1024) {
		sizek -= 1024 - basek;
		basek = 1024;
	}

	// need to figure out NV
	state->current += acpi_create_srat_mem((acpi_srat_mem_t *)state->current, (res->index & 0xf), basek, sizek, 1);
}

unsigned long acpi_fill_srat(unsigned long current)
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

unsigned long acpi_fill_slit(unsigned long current)
{
	/* need to find out the node num at first */
	/* fill the first 8 byte with that num */
	/* fill the next num*num byte with distance, local is 10, 1 hop mean 20, and 2 hop with 30.... */

	struct sys_info *sysinfox = (struct sys_info *)((CONFIG_RAMTOP) - sizeof(*sysinfox));
	u8 *ln = sysinfox->ln;


	u8 *p = (u8 *)current;
	int nodes = sysconf.nodes;
	int i,j;
	u32 hops;

	memset(p, 0, 8+nodes*nodes);
	*p = (u8) nodes;
	p += 8;

	for(i=0;i<nodes;i++) {
		for(j=0;j<nodes; j++) {
			if(i==j) {
				p[i*nodes+j] = 10;
			} else {
				hops = (((ln[i*NODE_NUMS+j]>>4) & 0x7)+1);
				p[i*nodes+j] = hops * 2 + 10;
			}
		}
	}

	current += 8+nodes*nodes;
	return current;
}

// moved from mb acpi_tables.c
static void intx_to_stream(u32 val, u32 len, u8 *dest)
{
	int i;
	for(i=0;i<len;i++) {
		*(dest+i) = (val >> (8*i)) & 0xff;
	}
}

static void int_to_stream(u32 val, u8 *dest)
{
	return intx_to_stream(val, 4, dest);
}

// used by acpi_tables.h
void update_ssdt(void *ssdt)
{
	u8 *BUSN;
	u8 *MMIO;
	u8 *PCIO;
	u8 *SBLK;
	u8 *TOM1;
	u8 *SBDN;
	u8 *HCLK;
	u8 *HCDN;
	u8 *CBST;
	u8 *CBBX;
	u8 *CBS2;
	u8 *CBB2;


	int i;
	u32 dword;
	msr_t msr;

	// the offset could be different if have different HC_NUMS, and HC_POSSIBLE_NUM and ssdt.asl
	BUSN = ssdt+0x3b; //+5 will be next BUSN
	MMIO = ssdt+0xe4; //+5 will be next MMIO
	PCIO = ssdt+0x36d; //+5 will be next PCIO
	SBLK = ssdt+0x4b2; // one byte
	TOM1 = ssdt+0x4b9; //
	SBDN = ssdt+0x4c3;//
	HCLK = ssdt+0x4d1; //+5 will be next HCLK
	HCDN = ssdt+0x57a; //+5 will be next HCDN
	CBBX = ssdt+0x61f; //
	CBST = ssdt+0x626;
	CBB2 = ssdt+0x62d; //
	CBS2 = ssdt+0x634;

	for(i=0;i<HC_NUMS;i++) {
		dword = sysconf.ht_c_conf_bus[i];
		int_to_stream(dword, BUSN+i*5);
	}

	for(i=0;i<(HC_NUMS*2);i++) { // FIXME: change to more chain
		dword = sysconf.conf_mmio_addrx[i]; //base
		int_to_stream(dword, MMIO+(i*2)*5);
		dword = sysconf.conf_mmio_addr[i]; //mask
		int_to_stream(dword, MMIO+(i*2+1)*5);
	}
	for(i=0;i<HC_NUMS;i++) { // FIXME: change to more chain
		dword = sysconf.conf_io_addrx[i];
		int_to_stream(dword, PCIO+(i*2)*5);
		dword = sysconf.conf_io_addr[i];
		int_to_stream(dword, PCIO+(i*2+1)*5);
	}

	*SBLK = (u8)(sysconf.sblk);

	msr = rdmsr(TOP_MEM);
	int_to_stream(msr.lo, TOM1);

	int_to_stream(sysconf.sbdn, SBDN);

	for(i=0;i<sysconf.hc_possible_num;i++) {
		int_to_stream(sysconf.pci1234[i], HCLK + i*5);
		int_to_stream(sysconf.hcdn[i],	   HCDN + i*5);
	}
	for(i=sysconf.hc_possible_num; i<HC_POSSIBLE_NUM; i++) { // in case we set array size to other than 8
		int_to_stream(0x00000000, HCLK + i*5);
		int_to_stream(0x20202020, HCDN + i*5);
	}

	*CBBX = (u8)(CONFIG_CBB);

	if(CONFIG_CBB == 0xff) {
		*CBST = (u8) (0x0f);
	} else {
		if((sysconf.pci1234[0] >> 12) & 0xff) { //sb chain on  other than bus 0
			*CBST = (u8) (0x0f);
		}
		else {
			*CBST = (u8) (0x00);
		}
	}

	if((CONFIG_CBB == 0xff) && (sysconf.nodes>32)) {
		 *CBS2 = 0x0f;
		 *CBB2 = (u8)(CONFIG_CBB-1);
	} else {
		*CBS2 = 0x00;
		*CBB2 = 0x00;
	}

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

static void update_sspr(void *sspr, u32 nodeid, u32 cpuindex)
{
	u8 *CPU;
	u8 *CPUIN;
	u8 *COREFREQ;
	u8 *POWER;
	u8 *TRANSITION_LAT;
	u8 *BUSMASTER_LAT;
	u8 *CONTROL;
	u8 *STATUS;
	unsigned offset = 0x94 - 0x7f;
	int i;

	CPU = sspr + 0x38;
	CPUIN = sspr + 0x3a;

	COREFREQ = sspr + 0x7f; //2 byte
	POWER = sspr + 0x82; //3 bytes
	TRANSITION_LAT = sspr + 0x87; //two bytes
	BUSMASTER_LAT = sspr + 0x8a; //two bytes
	CONTROL = sspr + 0x8d;
	STATUS = sspr + 0x8f;

	sprintf((char*)CPU, "%02x", (char)cpuindex);
	*CPUIN = (u8) cpuindex;

	for(i=0;i<sysconf.p_state_num;i++) {
		struct p_state_t *p_state = &sysconf.p_state[nodeid * 5 + i];
		intx_to_stream(p_state->corefreq, 2, COREFREQ + i*offset);
		intx_to_stream(p_state->power, 3, POWER + i*offset);
		intx_to_stream(p_state->transition_lat, 2, TRANSITION_LAT + i*offset);
		intx_to_stream(p_state->busmaster_lat, 2, BUSMASTER_LAT + i*offset);
		*((u8 *)(CONTROL + i*offset)) =(u8) p_state->control;
		*((u8 *)(STATUS + i*offset)) =(u8) p_state->status;
	}
}

extern const unsigned char AmlCode_sspr5[];
extern const unsigned char AmlCode_sspr4[];
extern const unsigned char AmlCode_sspr3[];
extern const unsigned char AmlCode_sspr2[];
extern const unsigned char AmlCode_sspr1[];

/* fixme: find one good way for different p_state_num */
unsigned long acpi_add_ssdt_pstates(acpi_rsdp_t *rsdp, unsigned long current)
{
	device_t cpu;
	int cpu_index = 0;

	acpi_header_t *ssdt;

	if(!sysconf.p_state_num) return current;

	void *AmlCode_sspr;
	switch(sysconf.p_state_num) {
		case 1: AmlCode_sspr = &AmlCode_sspr1; break;
		case 2: AmlCode_sspr = &AmlCode_sspr2; break;
		case 3: AmlCode_sspr = &AmlCode_sspr3; break;
		case 4: AmlCode_sspr = &AmlCode_sspr4; break;
		default: AmlCode_sspr = &AmlCode_sspr5; break;
	}

	for(cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		   (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			 continue;
		}
		printk(BIOS_DEBUG, "ACPI: pstate cpu_index=%02x, node_id=%02x, core_id=%02x\n", cpu_index, cpu->path.apic.node_id, cpu->path.apic.core_id);

		current	  = ALIGN(current, 16);
		ssdt = (acpi_header_t *)current;
		memcpy(ssdt, AmlCode_sspr, sizeof(acpi_header_t));
		current += ssdt->length;
		memcpy(ssdt, AmlCode_sspr, ssdt->length);
		update_sspr((void*)ssdt,cpu->path.apic.node_id, cpu_index);
		/* recalculate checksum */
		ssdt->checksum = 0;
		ssdt->checksum = acpi_checksum((unsigned char *)ssdt,ssdt->length);
		acpi_add_table(rsdp, ssdt);

		cpu_index++;
	}
	return current;
}
